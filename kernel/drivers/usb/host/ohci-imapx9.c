#include <linux/platform_device.h>
#include <linux/clk.h>
#include <mach/usb-phy.h>
#include <mach/hw_cfg.h>

struct imapx9_ohci_hcd {
	struct ohci_hcd *ohci;
	struct clk *bus_clk;
	struct clk *ref_clk;
	struct usb_cfg *cfg;
};

static struct usb_hcd *imap_ohci_hcd;

uint32_t imap_ohci_port_state(void)
{
	uint32_t temp;
	struct ohci_hcd *ohci = hcd_to_ohci(imap_ohci_hcd);

	temp = ohci_readl(ohci, &ohci->regs->roothub.portstatus[2]);
	return temp&0x1;
}

static int imapx9_ohci_start(struct usb_hcd *hcd)
{
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);
	int err;

	err = ohci_run(ohci);
	if (err < 0)
		ohci_stop(hcd);

	return err;
}

static int imapx9_ohci_reset(struct usb_hcd *hcd)
{
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);

	return ohci_init(ohci);
}

static const struct hc_driver imapx9_ohci_driver = {
	.description = hcd_name,
	.product_desc = "Infotm Ohci Controller",
	.hcd_priv_size = sizeof(struct ohci_hcd),
	.irq = ohci_irq,
	.flags = HCD_USB11 | HCD_MEMORY,
	.start = imapx9_ohci_start,
	.reset = imapx9_ohci_reset,
	.stop = ohci_stop,
	.shutdown = ohci_shutdown,
	.urb_enqueue = ohci_urb_enqueue,
	.urb_dequeue = ohci_urb_dequeue,
	.endpoint_disable = ohci_endpoint_disable,
	.get_frame_number = ohci_get_frame,
	/*
	 * root hub support
	 */
	.hub_status_data = ohci_hub_status_data,
	.hub_control = ohci_hub_control,
#ifdef CONFIG_PM
	.bus_suspend = ohci_bus_suspend,
	.bus_resume = ohci_bus_resume,
#endif
	.start_port_reset = ohci_start_port_reset,
};

static int imapx9_ohci_probe(struct platform_device *pdev)
{
	struct imapx9_ohci_hcd *imapx9;
	struct usb_hcd *hcd;
	struct resource *res;
	struct usb_cfg *cfg;
	int irq;
	int err;
	
	cfg = (struct usb_cfg *)(pdev->dev.platform_data);
	if (!cfg) {
		dev_err(&pdev->dev, "Ohci platform data miss\n");
		return -ENODEV;
	}

	imapx9 = devm_kzalloc(&pdev->dev, sizeof(struct imapx9_ohci_hcd),
				GFP_KERNEL);
	if (!imapx9)
		return -ENOMEM;

	hcd = usb_create_hcd(&imapx9_ohci_driver, &pdev->dev,
				dev_name(&pdev->dev));
	if (!hcd) {
		dev_err(&pdev->dev, "Unable to create HCD\n");
		return -ENOMEM;
	}
	platform_set_drvdata(pdev, imapx9);

	imapx9->bus_clk = clk_get_sys("imap-usb-ohci", "imap-usb");
	if (IS_ERR(imapx9->bus_clk)) {
		dev_err(&pdev->dev, "Can't get ohci clock\n");
		err = PTR_ERR(imapx9->bus_clk);
		goto fail_bus_clk;
	}
	err = clk_prepare_enable(imapx9->bus_clk);
	if (err)
		goto fail_bus_clk;
	imapx9->ref_clk = clk_get_sys("imap-usb-ref", "imap-usb");
	if (IS_ERR(imapx9->ref_clk)) {
		dev_err(&pdev->dev, "can't get phy ref clock\n");
		err = PTR_ERR(imapx9->ref_clk);
		goto fail_ref_clk;
	}
	clk_prepare_enable(imapx9->ref_clk);
	
	imapx9->cfg = cfg;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "Failed to get I/O memory\n");
		err = -ENXIO;
		goto fail_io;
	}
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);
	hcd->regs = devm_ioremap_resource(&pdev->dev, &pdev->resource[0]);
	if (IS_ERR(hcd->regs)) {
		dev_err(&pdev->dev, "Failed to remap I/O memory\n");
		err = -ENOMEM;
		goto fail_io;
	}

	imapx9->ohci = hcd_to_ohci(hcd);
	ohci_hcd_init(imapx9->ohci);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "Failed to get IRQ\n");
		err = -ENOMEM;
		goto fail_irq;
	}
	err = usb_add_hcd(hcd, irq, IRQF_DISABLED);
	if (err) {
		dev_err(&pdev->dev, "usb_add_hcd failed \r\n");
		goto fail_irq;
	}
	
	imap_ohci_hcd = hcd;
	return err;
fail_irq:
	devm_iounmap(&pdev->dev, hcd->regs);
fail_io:
	clk_disable_unprepare(imapx9->ref_clk);
fail_ref_clk:
	clk_disable_unprepare(imapx9->bus_clk);
fail_bus_clk:
	usb_put_hcd(hcd);
	return err;
}

static int imapx9_ohci_remove(struct platform_device *pdev)
{
	struct imapx9_ohci_hcd *imapx9 = platform_get_drvdata(pdev);
	struct usb_hcd *hcd = ohci_to_hcd(imapx9->ohci);

	usb_remove_hcd(hcd);
	clk_disable_unprepare(imapx9->bus_clk);
	devm_iounmap(&pdev->dev, hcd->regs);
	usb_put_hcd(hcd);
	return 0;
}

static void imapx9_ohci_hcd_shutdown(struct platform_device *pdev)
{
	struct imapx9_ohci_hcd *imapx9 = platform_get_drvdata(pdev);
	struct usb_hcd *hcd = ohci_to_hcd(imapx9->ohci);

	if (hcd->driver->shutdown)
		hcd->driver->shutdown(hcd);
}

#ifdef CONFIG_PM
static int imapx9_ohci_hcd_suspend(struct device *dev)
{
	struct imapx9_ohci_hcd *imapx9 =
		platform_get_drvdata(to_platform_device(dev));
	struct ohci_hcd *ohci = imapx9->ohci;
	struct usb_hcd *hcd = ohci_to_hcd(ohci);

	return ohci_suspend(hcd, device_may_wakeup(dev));
}

static int imapx9_ohci_hcd_resume(struct device *dev)
{
	struct imapx9_ohci_hcd *imapx9 =
		platform_get_drvdata(to_platform_device(dev));
	struct ohci_hcd *ohci = imapx9->ohci;
	struct usb_hcd *hcd = ohci_to_hcd(ohci);
	int clk_rate;

	clk_prepare_enable(imapx9->bus_clk);
	clk_prepare_enable(imapx9->ref_clk);
	clk_rate = clk_get_rate(imapx9->ref_clk);
	host_phy_config(clk_rate, 1);

	ohci_resume(hcd, false);
	return 0;
}

static const struct dev_pm_ops imapx9_ohci_pm_ops = {
	.suspend	= imapx9_ohci_hcd_suspend,
	.resume     = imapx9_ohci_hcd_resume,
};
#endif

static struct platform_driver ohci_imapx9_driver = {
	.probe		= imapx9_ohci_probe,
	.remove		= imapx9_ohci_remove,
	.shutdown	= imapx9_ohci_hcd_shutdown,
	.driver		= {
		.name	= "imap-ohci",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm		= &imapx9_ohci_pm_ops,
#endif
	},
};
