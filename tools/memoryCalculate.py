#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import logging
import sys
import os

logging.basicConfig(level=logging.ERROR,
                format='%(levelname)4s: %(message)s',
#               %(asctime)s %(filename)s[%(lineno)4d]
                datefmt='%H:%M:%S',
#                filename='myapp.log',
                filemode='w')

def enum(**enums):
    return type('Enum', (), enums)

PORTINFO = enum(BufferCnt=0, Mem=1, Enabled=2)

def loadJson(jsonFile):
    totalMemory = 0
    ipuMemory = 0
    parser = IPUMemoryParser()

    f = open(jsonFile, 'r')
    path = json.load(f)

    for i in path:
        ipu = path[i]
        if ipu['ipu'] in parser.getIPUs():
            parser.updateBindInfo(i, ipu)

    for i in path:
        ipu = path[i]
        ipuMemory = 0
        logging.info("name:"+i+", id:"+ipu['ipu'])
        if ipu['ipu'] in parser.getIPUs():
            ipuMemory += parser.parse(i, ipu)
        else:
            logging.error("Error: can not find ipu:" + ipu['ipu'])
        totalMemory += ipuMemory

    logging.info("total memory:" + str(totalMemory/1024) + "KBytes(" + str(totalMemory) + "Bytes)")
    parser.showMemoryInfo(totalMemory)
    parser.resetParser()

    return totalMemory

def updatePortMemory(ports, portInfo):
    for port in ports.keys():
        if port in portInfo.keys():
            memory = getPortMemory(ports[port], portInfo[port][PORTINFO.BufferCnt])
            if memory > 0:
                portInfo[port][PORTINFO.Mem] = memory
            portInfo[port][PORTINFO.Enabled] = 1
            logging.debug("port:"+port+", memory:"+str(portInfo[port][PORTINFO.Mem]) + ", bufferCnt:" + str(portInfo[port][PORTINFO.BufferCnt]) + ", enabled:" + str(portInfo[port][PORTINFO.Enabled]))

def getPortsMemory(ports):
    memory = 0
    for port in ports.keys():
        if ports[port][PORTINFO.Enabled] == 1:
            memory += ports[port][PORTINFO.Mem]
            logging.debug(port + ": " + str(ports[port][PORTINFO.Mem]))
    return memory

def getPortMemory(port, bufCnt):
    memory = 0
    #formate:[rate, base] rate/base
    rates = {'nv12':[3, 2], 'NV12':[3, 2], 'nv21':[3, 2], 'NV21':[3, 2], 'bpp2':[1, 4], 'BPP2':[1, 4], 'rgb565':[18, 8], 'RGB565':[18, 8], 'yuyv422':[2, 1], 'YUYV422':[2, 1], 'rgba8888':[4, 1],  'RGBA8888':[4, 1]}

    if 'w' in port.keys() and 'h' in port.keys():
        memory = port['w'] * port['h']
    else:
        return memory

    if 'pixel_format' in port.keys() and port['pixel_format'] in rates.keys():
        memory = memory * rates[port['pixel_format']][0] / rates[port['pixel_format']][1]
    else:
        memory = memory * 3 / 2
    memory = memory * bufCnt
    return memory

def getVideoSize(ipuName, ipu, revertBindInfo):
    w = 0
    h = 0
    if ipuName in revertBindInfo.keys():
        w = revertBindInfo[ipuName][3]
        h = revertBindInfo[ipuName][4]
    if w <= 0 or h <= 0:
        logging.error("w: " + str(w) + ", h: " + str(h))
        return {'w':0, 'h':0}
    if 'port' in ipu.keys() and 'frame' in ipu['port'].keys():
        if 'pip_w' in ipu['port']['frame'].keys() and 'pip_h' in ipu['port']['frame'].keys():
            w = ipu['port']['frame']['pip_w']
            h = ipu['port']['frame']['pip_h']
    logging.debug("w: " + str(w) + ", h: " + str(h))
    return {'w':w, 'h':h}

class IPUMemoryParser(object):
    revertBindInfo = {}
    memoryInfo = {}
    bindInfo = {}

    def __init__(self):
        self.parserFuns = {
                        "v2500": self.getISPMemory,
                        "v2505": self.getISPMemory,
                        "ispost": self.getISPostv1Memory,
                        "ispostv2": self.getISPostv2Memory,
                        "marker": self.getMarkerMemory,
                        "h2": self.getH2Memroy,
                        "h1264": self.getH1Memroy,
                        "g1264": self.getG1Memory,
                        "g2": self.getG2Memory,
                        "h1jpeg": self.getH1jpegMemory,
                        "jenc": self.getJencMemory,
                        "ffvdec": self.getFFvdecMemory,
                        "ffphoto": self.getFFphotoMemory,
                        "pp": self.getPpMemory,
                        "ids": self.getDefaultMemory,
                        "bufsync": self.getDefaultMemory,
                        "dg-frame": self.getDgframeMemory,
                        "fodetv2": self.getFodetv2Memory,
                        "vamovement": self.getDefaultMemory,
                        "mvmovement": self.getDefaultMemory,
                        "vaqrscanner": self.getDefaultMemory,
                        "swc": self.getSwcMemory,
                        "softlayer": self.getDefaultMemory,
                        "filesink": self.getDefaultMemory,
                        "filesource": self.getFilesourceMemory,
                        "v4l2": self.getV4l2Memory,
                        "vencoder": self.getVencoderMemory,
                            }
        self.resetParser()

    def getISPostv1Memory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'dn':[3, 0, 0], 'ss0':[3, 0, 0], 'ss1':[3, 0, 0]}

        if 'args' in ipu.keys() and 'buffers' in ipu['args'].keys():
            portsInfo['dn'][PORTINFO.BufferCnt] = ipu['args']['buffers']
            portsInfo['ss0'][PORTINFO.BufferCnt] = ipu['args']['buffers']
            portsInfo['ss1'][PORTINFO.BufferCnt] = ipu['args']['buffers']

        updatePortMemory(ipu['port'], portsInfo)

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getISPostv2Memory(self, ipuName, ipu):
        ipuMemory = 0
        fileCnt = 0
        fileSize = 30*1024
        portsInfo = {'dn':[1, 0, 0], 'ss0':[3, 0, 0], 'ss1':[3, 0, 0], 'uo':[3, 0, 0], 'internal':[1, 0, 0]}

        if 'args' in ipu.keys():
            if 'buffers' in ipu['args'].keys():
                portsInfo['uo'][PORTINFO.BufferCnt] = ipu['args']['buffers']
                portsInfo['ss0'][PORTINFO.BufferCnt] = ipu['args']['buffers']
                portsInfo['ss1'][PORTINFO.BufferCnt] = ipu['args']['buffers']
            if 'linkmode' in ipu['args'].keys() and ipu['args']['linkmode'] > 0:
                portsInfo['uo'][PORTINFO.BufferCnt] = 0

        updatePortMemory(ipu['port'], portsInfo)

        if 'args' in ipu.keys():
            for name in ipu['args'].keys():
                if (name.find("lc_grid_file_name", 0) >= 0) or (name.find("lc_geometry_file_name", 0) >= 0):
                    fileCnt += 1

        if fileCnt > 0:
            portsInfo['internal'][PORTINFO.Enabled] = 1
            portsInfo['internal'][PORTINFO.Mem] += fileCnt * fileSize

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getISPMemory(self, ipuName, ipu):
        ipuMemory = 0
        #PortName:[bufferCnt,allocMemory,enabled]
        portsInfo = {'out':[3, 0, 0], 'his':[0, 128*1024, 0], 'cap':[1, 0, 0], 'ddk':[1, 0, 0]}

        if 'args' in ipu.keys() and 'nbuffers' in ipu['args'].keys():
            portsInfo['out'][PORTINFO.BufferCnt] = ipu['args']['nbuffers']

        updatePortMemory(ipu['port'], portsInfo)

        if portsInfo['out'][PORTINFO.Mem] > portsInfo['cap'][PORTINFO.Mem]:
            portsInfo['cap'][PORTINFO.Mem] = 0
        else:
            portsInfo['out'][PORTINFO.Mem] = 0

        portsInfo['ddk'][PORTINFO.Enabled] = 1
        portsInfo['ddk'][PORTINFO.Mem] = 1024 * 1024
        portsInfo['ddk'][PORTINFO.Mem] += portsInfo['out'][PORTINFO.Mem]
        portsInfo['out'][PORTINFO.Mem] = 0
        portsInfo['ddk'][PORTINFO.Mem] += portsInfo['cap'][PORTINFO.Mem]
        portsInfo['cap'][PORTINFO.Mem] = 0
        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getMarkerMemory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'out':[1, 0, 0], 'internal':[1, 0, 0]}

        #buffer for freetype to draw
        portsInfo['internal'][PORTINFO.Mem] = 120 * 20 * 4
        portsInfo['internal'][PORTINFO.Enabled] = 1

        # fixed buffer cnt
        portsInfo['out'][PORTINFO.BufferCnt] = 2

        updatePortMemory(ipu['port'], portsInfo)

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getH2Memroy(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'stream':[1, 0, 0], 'dpb':[1, 0, 0],'ddk':[1, 0, 0]}
        video = getVideoSize(ipuName, ipu, self.revertBindInfo)
        w = video['w']
        h = video['h']

        if w <= 0 or h <= 0:
            logging.error("w: " + str(w) + ", h: " + str(h))
            return ipuMemory

        portsInfo['stream'][PORTINFO.Enabled] = 1
        portsInfo['stream'][PORTINFO.Mem] = 3145728

        if 'args' in ipu.keys() and 'buffer_size' in ipu['args'].keys():
            portsInfo['stream'][PORTINFO.Mem] = ipu['args']['buffer_size']

        #DPB Buffer
        w = ((w + 63) >> 6) << 6
        h = (((h + 63) >> 6) << 6)

        width_4n = ((w + 15) / 16) * 4
        height_4n = ((h + 15) / 16) * 4
        internalImageLumaSize = (w * h + width_4n * height_4n)
        refNum = 2
        portsInfo['dpb'][PORTINFO.Enabled] = 1
        portsInfo['dpb'][PORTINFO.Mem] += refNum * internalImageLumaSize + refNum * internalImageLumaSize / 2
        logging.debug("dpb buffer: " + str(refNum * w * h * 3 / 2))

        portsInfo['ddk'][PORTINFO.Enabled] = 1
        #table buffer
        portsInfo['ddk'][PORTINFO.Mem] += ((h + 63)/64 + 1) * 4 + 7
        #compress coeff SACN buffer
        portsInfo['ddk'][PORTINFO.Mem] += 288 * 1024 / 8
        #compressor buffer
        portsInfo['ddk'][PORTINFO.Mem] += refNum * ((w + 63) / 64) * ((h + 63) / 64) * 8

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getH1Memroy(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'stream':[1, 0, 0], 'dpb':[1, 0, 0],'ddk':[1, 0, 0]}
        video = getVideoSize(ipuName, ipu, self.revertBindInfo)
        w = video['w']
        h = video['h']

        if w <= 0 or h <= 0:
            logging.error("w: " + str(w) + ", h: " + str(h))
            return ipuMemory

        portsInfo['stream'][PORTINFO.Enabled] = 1
        portsInfo['stream'][PORTINFO.Mem] = 2359296

        if 'args' in ipu.keys() and 'buffer_size' in ipu['args'].keys():
            portsInfo['stream'][PORTINFO.Mem] = ipu['args']['buffer_size']

        #DPB Buffer
        refNum = 2
        if 'args' in ipu.keys() and 'enable_longterm' in ipu['args'].keys():
            if ipu['args']['enable_longterm'] == 1:
                refNum = 3
        portsInfo['dpb'][PORTINFO.Enabled] = 1
        portsInfo['dpb'][PORTINFO.Mem] += refNum * w * h * 3 / 2
        logging.debug("dpb buffer: " + str(refNum * w * h * 3 / 2))

        width = (w + 15) / 16
        height = (h + 15) / 16
        mbTotal = width * height

        portsInfo['ddk'][PORTINFO.Enabled] = 1
        portsInfo['ddk'][PORTINFO.Mem] = 0

        #nal size table
        portsInfo['ddk'][PORTINFO.Mem] += (4 * (height+4) + 7) & (~7)
        #CABAC context tables
        portsInfo['ddk'][PORTINFO.Mem] += 52 * 2 * 464
        #MV output table
        portsInfo['ddk'][PORTINFO.Mem] += mbTotal * 48
        #Segmentation map
        portsInfo['ddk'][PORTINFO.Mem] += (mbTotal*4 + 63) / 64 * 8

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getH1jpegMemory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'stream':[1, 0, 0], 'thumbnail':[1, 0, 0], 'slice':[1, 0, 0], 'internal':[1, 0, 0]}
        video = getVideoSize(ipuName, ipu, self.revertBindInfo)
        w = video['w']
        h = video['h']

        if w <= 0 or h <= 0:
            logging.error("w: " + str(w) + ", h: " + str(h))
            return ipuMemory

        #stream buffer = framebuffer * 75%
        portsInfo['stream'][PORTINFO.Enabled] = 1
        portsInfo['stream'][PORTINFO.Mem] = w * h * 3 * 3 / 2 / 4

        #thumbnail buffer = thumbnail framebuffer * 75%
        if 'args' in ipu.keys() and 'thumbnailWidth' in ipu['args'].keys() and 'thumbnailHeight' in ipu['args'].keys():
            tw = ipu['args']['thumbnailWidth']
            th = ipu['args']['thumbnailHeight']
            portsInfo['thumbnail'][PORTINFO.Enabled] = 1
            portsInfo['thumbnail'][PORTINFO.Mem] = tw * th * 3 * 3 / 2 /4

        #slice buffer = framebuffer / 6(slice Count)
        portsInfo['slice'][PORTINFO.Enabled] = 1
        portsInfo['slice'][PORTINFO.Mem] = w * h * 3 / 2 / 6

        #backend buffer = framebuffer
        if 'args' in ipu.keys() and 'usebackupbuf' in ipu['args'].keys():
            portsInfo['internal'][PORTINFO.Enabled] = 1
            portsInfo['internal'][PORTINFO.Mem] = w * h * 3 / 2

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getJencMemory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'stream':[1, 0, 0]}
        video = getVideoSize(ipuName, ipu, self.revertBindInfo)
        w = video['w']
        h = video['h']

        #stream buffer = framebuffer * 75%
        portsInfo['stream'][PORTINFO.Enabled] = 1
        portsInfo['stream'][PORTINFO.Mem] = w * h * 3 * 3 / 2 / 4

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getG1Memory(self, ipuName, ipu):
        ipuMemory = 0
        refFrm = 1
        totFrm = refFrm + 1
        portsInfo = {'stream':[1, 0, 0], 'dpb':[1, 0, 0]}

        if ipuName not in self.revertBindInfo.keys():
            portsInfo['stream'][PORTINFO.Enabled] = 1
            portsInfo['stream'][PORTINFO.Mem] = 2375680

        w = ipu['port']['frame']['w']
        h = ipu['port']['frame']['h']

        picWidthInMbs = (w + 15) / 16
        picHeightInMbs = (h + 15) / 16
        picSizeInMbs = picWidthInMbs * picHeightInMbs

        portsInfo['dpb'][PORTINFO.Enabled] = 1
        portsInfo['dpb'][PORTINFO.Mem] = picSizeInMbs * (384 + 64) * totFrm

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getG2Memory(self, ipuName, ipu):
        ipuMemory = 0
        refFrm = 1
        totFrm = refFrm + 2 + 1
        portsInfo = {'stream':[1, 0, 0], 'dpb':[1, 0, 0], 'rbm':[1, 0, 0]}

        if ipuName not in self.revertBindInfo.keys():
            portsInfo['stream'][PORTINFO.Enabled] = 1
            portsInfo['stream'][PORTINFO.Mem] = 2375680

        w = ipu['port']['frame']['w']
        h = ipu['port']['frame']['h']

        blockSize = 6
        wCtbs = (w + (1 << blockSize) - 1) >> blockSize
        hCtbs = (h + (1 << blockSize) - 1) >> blockSize
        devMem = (wCtbs * hCtbs * (1 << (2 * (blockSize - 4))) * 16)

        portsInfo['dpb'][PORTINFO.Enabled] = 1
        portsInfo['dpb'][PORTINFO.Mem] = (w * h * 3 / 2 + devMem) * totFrm

        portsInfo['rbm'][PORTINFO.Enabled] = 1
        portsInfo['rbm'][PORTINFO.Mem] = (w * h * 3 / 2) * totFrm

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getFFvdecMemory(self, ipuName, ipu):
        ipuMemory = 0
        refFrm = 1
        totFrm = refFrm + 1
        portsInfo = {'stream':[1, 0, 0], 'frame':[1, 0, 0], 'dpb':[1, 0, 0]}

        if ipuName not in self.revertBindInfo.keys():
            portsInfo['stream'][PORTINFO.Enabled] = 1
            portsInfo['stream'][PORTINFO.Mem] = 524288 * 3

        w = ipu['port']['frame']['w']
        h = ipu['port']['frame']['h']

        portsInfo['dpb'][PORTINFO.Enabled] = 1
        portsInfo['dpb'][PORTINFO.Mem] = (w * h * 3 / 2) * totFrm

        portsInfo['frame'][PORTINFO.Enabled] = 1
        portsInfo['frame'][PORTINFO.Mem] = (w * h * 3 / 2) * 3

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getFFphotoMemory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'stream':[1, 0, 0], 'frame':[1, 0, 0], 'dpb':[1, 0, 0]}

        if ipuName not in self.revertBindInfo.keys():
            portsInfo['stream'][PORTINFO.Enabled] = 1
            portsInfo['stream'][PORTINFO.Mem] = 3145728

        w = ipu['port']['frame']['w']
        h = ipu['port']['frame']['h']

        portsInfo['frame'][PORTINFO.Enabled] = 1
        portsInfo['frame'][PORTINFO.Mem] = w * h * 3 / 2

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getPpMemory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'out':[3, 0, 0]}

        w = ipu['port']['out']['w']
        h = ipu['port']['out']['h']
        if 'embezzle' not in ipu['port']['out'].keys():
            updatePortMemory(ipu['port'], portsInfo)

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getDgframeMemory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'out':[6, 0, 0]}
        modeFrames = {'vacant':8, 'picture':2, 'yuvfile':2}

        if 'args' in ipu.keys() and 'frames' in ipu['args'].keys():
            if ipu['args']['frames'] == 0:
                if 'mode' in ipu['args'].keys() and ipu['args']['mode'] in modeFrames.keys():
                    portsInfo['out'][PORTINFO.BufferCnt] = modeFrames[ipu['args']['mode']]
            else:
                portsInfo['out'][PORTINFO.BufferCnt] = ipu['args']['frames']

        updatePortMemory(ipu['port'], portsInfo)
        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getV4l2Memory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'out':[4, 0, 0]}

        updatePortMemory(ipu['port'], portsInfo)
        if 'args' in ipu.keys() and 'pixfmt' in ipu['args'].keys():
            if ipu['args']['pixfmt'] == 'mjpeg' or ipu['args']['pixfmt'] == 'MJPEG':
                logging.info("mjpeg mode memory size decided by sensor")
            if ipu['args']['pixfmt'] == 'h264' or ipu['args']['pixfmt'] == 'H264':
                logging.info("H264 mode memory size decided by sensor")

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getFilesourceMemory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo ={'out':[1, 458752, 1]}

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getFodetv2Memory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'ORout':[1, 0, 0], 'internal':[1, 0, 0]}

        w = ipu['port']['ORout']['w']
        h = ipu['port']['ORout']['h']

        portsInfo['ORout'][PORTINFO.Enabled] = 1
        portsInfo['ORout'][PORTINFO.Mem] = w * h * 3 / 2

        portsInfo['internal'][PORTINFO.Enabled] = 1
        portsInfo['internal'][PORTINFO.Mem] = 960*1024 + 52*1024 + 2*50*1024
        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getSwcMemory(self, ipuName, ipu):
        ipuMemory = 0
        portsInfo = {'out':[3, 0, 0], 'dn':[3, 0, 0], 'internal':[1, 0, 0]}
        w = 0
        h = 0

        fileCnt = 0
        fileSize = 1024 * 1024
        if 'args' in ipu.keys():
            for name in ipu['args'].keys():
                if (name.find("lc_grid_file_name", 0) >= 0) or (name.find("lc_geometry_file_name", 0) >= 0):
                    fileCnt += 1

        if fileCnt == 0:
            fileCnt = 1

        portsInfo['internal'][PORTINFO.Enabled] = 1
        portsInfo['internal'][PORTINFO.Mem] += fileCnt * fileSize

        if 'args' in ipu.keys():
            if 'buffers' in ipu['args'].keys():
                portsInfo['dn'][PORTINFO.BufferCnt] = ipu['args']['buffers']
            if 'w' in ipu['args'].keys():
                w = ipu['args']['w']
            if 'h' in ipu['args'].keys():
                h = ipu['args']['h']

        if w == 0 and h == 0:
            w = ipu['port']['out']['w']
            h = ipu['port']['out']['h']

        if w > 0 and h > 0:
            portsInfo['out'][PORTINFO.Enabled] = 1
            portsInfo['out'][PORTINFO.Mem] = portsInfo['out'][PORTINFO.BufferCnt] * w * h * 3 / 2
            portsInfo['dn'][PORTINFO.Enabled] = 1
            portsInfo['dn'][PORTINFO.Mem] = portsInfo['dn'][PORTINFO.BufferCnt] * w * h * 3 / 2

        ipuMemory = getPortsMemory(portsInfo)
        self.updateMemInfo(ipuName, portsInfo)
        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getVencoderMemory(self, ipuName, ipu):
        ipuMemory = 0

        if 'args' in ipu.keys():
            if "encode_type" in ipu['args'].keys():
                if ipu['args']['encode_type'] == "h264":
                    ipuMemory = self.getH1Memroy(ipuName, ipu)
                elif ipu['args']['encode_type'] == "h265":
                    ipuMemory = self.getH2Memroy(ipuName, ipu)
                else:
                    logging.error("encode_type " + str(ipu['args']['encode_type']) + " error")
            else:
                logging.error("can not find encode_type attribute")

        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def getDefaultMemory(self, ipuName, ipu):
        ipuMemory = 0

        logging.info(ipuName + " memory: " + str(ipuMemory / 1024) + "KByte")
        return ipuMemory

    def updateMemInfo(self, ipuName, portsInfo):
        for portName in portsInfo.keys():
            self.memoryInfo[ipuName]['detail'][str(portName)] = str(portsInfo[portName][PORTINFO.Mem] // 1024) + "KB"

    def updateBindInfo(self, ipuName, ipu):
        if 'port' in ipu.keys():
            for port in ipu['port'].keys():
                w = 0
                h = 0
                if 'w' in ipu['port'][port]:
                    w = ipu['port'][port]['w']
                if 'h' in ipu['port'][port]:
                    h = ipu['port'][port]['h']
                if 'bind' in ipu['port'][port]:
                    for bindIPU in ipu['port'][port]['bind'].keys():
                        key = bindIPU
                        value = [ipuName, ipu['port'][port]['bind'][bindIPU], port, w, h]
                        self.revertBindInfo[key] = value

                    if len(ipu['port'][port]['bind'].keys()) > 0:
                        if ipuName in self.bindInfo.keys():
                            self.bindInfo[ipuName].extend(ipu['port'][port]['bind'].keys())
                        else:
                            self.bindInfo[ipuName] = list(ipu['port'][port]['bind'].keys())
                #logging.debug(ipuName + " revertBindInfo:" + str(self.revertBindInfo))

    def parse(self, ipuName, ipu):
        memory = 0
        self.memoryInfo[ipuName] = {'total':memory, 'detail':{}, 'id':ipu['ipu']}
        #print self.memoryInfo
        memory = self.parserFuns[ipu['ipu']](ipuName, ipu)
        self.memoryInfo[ipuName]['total'] = str(memory // 1024) + "KB"
        return memory

    def showMemoryInfo(self, totalMemory):
        showHeader = True
        showedIPU = []
        queue = []

        print
        # find root ipu, which not be binded to other ipu
        for key in self.memoryInfo.keys():
            if key not in showedIPU and key not in self.revertBindInfo.keys():
                self.showIPUsMemoryInfo(key, showedIPU, queue)

        # 1. pop ipus in queues
        # 2. visit ipus which bind to those ipus, and add binded ipus to queue
        # 3. goto step 1, untile no ipu in queue
        while len(queue) > 0:
            for key in queue:
                if key in self.bindInfo.keys():
                    for k in self.bindInfo[key]:
                        if k not in showedIPU:
                            self.showIPUsMemoryInfo(k, showedIPU, queue)
                queue.remove(key)

        for key in self.memoryInfo.keys():
            if key not in showedIPU:
                self.showIPUsMemoryInfo(key, showedIPU, queue)

        print ("------------------------------------------------------------------------------------")
        print (6 * 12 * ' ' + "%12s"%('total').upper())
        print (6 * 12 * ' ' + "%12s"%(str(totalMemory // 1024) + 'KB'))

    def showIPUsMemoryInfo(self, key, showedIPU, queue):
        self.showIPUMemoryInfo(key, True)
        showedIPU.append(key)
        queue.append(key)

        if key not in self.memoryInfo.keys():
            return
        for k in self.memoryInfo.keys():
            if self.memoryInfo[k]['id'] == self.memoryInfo[key]['id'] and k not in showedIPU:
                self.showIPUMemoryInfo(k, False)
                showedIPU.append(k)
                queue.append(key)
        print ('')

    def showIPUMemoryInfo(self, key, showHeader):
        if key not in self.memoryInfo.keys():
            return
        # SHOW HEAD INFO
        if showHeader == True:
            padding = ''
            if (len(self.memoryInfo[key]['id']) < 16):
                padding = ((16-len(self.memoryInfo[key]['id']))*'-')
            print ("-------------------------------------%s%s-------------------------------"%(self.memoryInfo[key]['id'], padding))

        # SHOW TITLE INFO
        detList = []

        if showHeader == True:
            sys.stdout.write ("%12s"%'name'.upper())
        detList.append(key)

        for detKey in self.memoryInfo[key]['detail'].keys():
            if showHeader == True:
                sys.stdout.write ("%12s"%detKey.upper())
            detList.append(self.memoryInfo[key]['detail'][detKey])

        if len(detList) == 1:
            if showHeader == True:
                sys.stdout.write  ("%12s"%('internal').upper())
            detList.append('0KB')

        if showHeader == True:
            sys.stdout.flush()

        if showHeader == True:
            print ((6 - len(detList)) * 12 * ' ' + "%12s"%('total').upper())

        # SHOW MEM INFO
        for detVal in detList:
            sys.stdout.write ("%12s"%str(detVal),)
        print ((6 - len(detList)) * 12 * ' ' + "%12s"%(self.memoryInfo[key]['total']))

    def getIPUs(self):
        return self.parserFuns.keys()

    def resetParser(self):
        self.revertBindInfo = {}
        self.memoryInfo = {}
        self.bindInfo = {}

if __name__ == '__main__':
    if len(sys.argv) == 1:
        logging.info("If want to set json file, run './memory_calculate.py XXX.json', otherwise set path.json")
        if os.path.exists("path.json"):
            loadJson("path.json")
        else:
            logging.error("run './memory_calculate.py XXX.json', otherwise use path.json")

    for i in range(1, len(sys.argv)):
        logging.info( "path "+ str(i) + ":" + str(sys.argv[i]))
        loadJson(sys.argv[i])
