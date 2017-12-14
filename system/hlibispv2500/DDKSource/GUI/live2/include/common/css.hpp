#ifndef CSS_H
#define CSS_H

namespace VisionLive
{

enum CSS {DEFAULT_CSS, CUSTOM_CSS};

#define CSS_DEFAULT_QMAINWINDOW QString("QMainWindow { }")
#define CSS_DEFAULT_QMANUBAR QString("QMenuBar { }")
#define CSS_DEFAULT_QMENU QString("QMenu { }")
#define CSS_DEFAULT_QTABWIDGET QString("QTabWidget { }")
#define CSS_DEFAULT_QGROUPBOX QString("QGroupBox { }")
#define CSS_DEFAULT_QLABEL QString("QLabel { }")
#define CSS_DEFAULT_QSPINBOX QString("QSpinBox { }")
#define CSS_DEFAULT_QDOUBLESPINBOX QString("QDoubleSpinBox { }")
#define CSS_DEFAULT_QCOMBOBOX QString("QComboBox QAbstractItemView { }")
#define CSS_DEFAULT_QCHECKBOX QString("QCheckBox { }")
#define CSS_DEFAULT_QTEXTEDIT QString("QTextEdit { }")
#define CSS_DEFAULT_QTREEVIEW QString("QTreeView { }")
#define CSS_DEFAULT_QTABLEWIDGET QString("QTableWidget { }")
#define CSS_DEFAULT_QHEADERVIEW QString("QHeaderView { }")
#define CSS_DEFAULT_QSCROLLAREA QString("QScrollArea { }")
#define CSS_DEFAULT_QWTPLOT QString("QwtPlot { }")
#define CSS_DEFAULT_QTOOLBAR QString("QToolBar { }")
#define CSS_DEFAULT_QINPUTDIALOG QString("QInputDialog { }")
#define CSS_DEFAULT_QMESSAGEBOX QString("QMessageBox { }")
#define CSS_DEFAULT_QSTATUSBAR QString("QStatusBar { }")
#define CSS_DEFAULT_QGRAPHICSVIEW QString("QGraphicsView { }")
#define CSS_DEFAULT_QLINEEDIT QString("QLineEdit { }")
#define CSS_DEFAULT_QRADIOBUTTON QString("QRadioButton { }")

#define CSS_DEFAULT_STYLESHEET CSS_DEFAULT_QMAINWINDOW + CSS_DEFAULT_QMANUBAR + \
								CSS_DEFAULT_QMENU + CSS_DEFAULT_QTABWIDGET + CSS_DEFAULT_QGROUPBOX + \
								CSS_DEFAULT_QLABEL + CSS_DEFAULT_QSPINBOX + CSS_DEFAULT_QDOUBLESPINBOX + \
								CSS_DEFAULT_QCOMBOBOX + CSS_DEFAULT_QCHECKBOX + \
								CSS_DEFAULT_QTEXTEDIT + CSS_DEFAULT_QTREEVIEW + CSS_DEFAULT_QTABLEWIDGET + \
								CSS_DEFAULT_QHEADERVIEW + CSS_DEFAULT_QSCROLLAREA + CSS_DEFAULT_QWTPLOT + \
								CSS_DEFAULT_QTOOLBAR + CSS_DEFAULT_QINPUTDIALOG + CSS_DEFAULT_QMESSAGEBOX + \
								CSS_DEFAULT_QSTATUSBAR + CSS_DEFAULT_QGRAPHICSVIEW + CSS_DEFAULT_QLINEEDIT + \
								CSS_DEFAULT_QRADIOBUTTON

//#define CSS_CUSTOM_QMAINWINDOW QString(" \
//								QMainWindow \
//								{ \
//									background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #00CC99, stop: 1 #000000); \
//								}")
//
//#define CSS_CUSTOM_QMANUBAR QString(" \
//							 QMenuBar \
//							 { \
//								background-color: #00664C; \
//								color: #FFFFFF; \
//							 } \
//						     QMenuBar::item \
//							 { \
//								spacing: 3px; \
//								padding: 1px 4px; \
//								background: transparent; \
//								border-radius: 2px; \
//							 } \
//							 QMenuBar::item:selected \
//							 { \
//								background: #00CC99; \
//							 } \
//							 QMenuBar::item:pressed { \
//								background: #000000; \
//								color: #FFFFFF; \
//							 }")
//
//#define CSS_CUSTOM_QMENU QString(" \
//						  QMenu \
//						  { \
//							background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #00CC99, stop: 1 #000000);\
//						  } \
//						  QMenu::item::selected \
//						  { \
//							 background-color: transparent; \
//							 color: #FFFFFF; \
//						  } \
//						  QMenu::item::!selected \
//						  { \
//							background-color: transparent; \
//							color: black; \
//						  } \
//						  QMenu::item::!enabled \
//						  { \
//							color: #BEBDBF; \
//						  }")
//
//#define CSS_CUSTOM_QTABWIDGET QString(" \
//							   QTabWidget::pane \
//							   { \
//								 border: 1px solid transparent; \
//								 background: transparent; \
//							   } \
//							   QTabBar::tab \
//							   { \
//							 	 background: transparent; \
//								 border: 1px solid white; \
//								 border-bottom-color: transparent; \
//								 border-top-left-radius: 4px; \
//								 border-top-right-radius: 4px; \
//				  				 padding: 2px; \
//								 color: #FFFFFF; \
//							   } \
//							   QTabBar::tab:hover \
//							   { \
//								 background: #003D2E; \
//							   } \
//							   QTabBar::tab:selected \
//							   { \
//								 background: #003D2E; \
//							   } \
//							   QTabBar::tab:!selected \
//							   { \
//								 margin-top: 5px; \
//							   }")
//
//#define CSS_CUSTOM_QTABWIDGET_06_OPACITY QString(" \
//							   QTabWidget::pane \
//							   { \
//								 border: 1px solid transparent; \
//								 background: rgba(0, 0, 0, 0.6); \
//							   } \
//							   QTabBar::tab \
//							   { \
//							 	 background: rgba(0, 0, 0, 0.6); \
//								 border: 1px solid white; \
//								 border-bottom-color: transparent; \
//								 border-top-left-radius: 4px; \
//								 border-top-right-radius: 4px; \
//								 min-width: 12ex; \
//				  				 padding: 2px; \
//								 color: #FFFFFF; \
//							   } \
//							   QTabBar::tab:hover \
//							   { \
//								 background: #003D2E; \
//							   } \
//							   QTabBar::tab:selected \
//							   { \
//								 background: #003D2E; \
//							   } \
//							   QTabBar::tab:!selected \
//							   { \
//								 margin-top: 5px; \
//							   }")
//
//#define CSS_CUSTOM_QGROUPBOX QString(" \
//							  QGroupBox \
//							  { \
//								border: 2px solid #FFFFFF; \
//								border-radius: 5px; \
//								margin-top: 2ex; \
//							    color: #FFFFFF; \
//							  } \
//							  QGroupBox::title \
//							  { \
//								subcontrol-origin: margin; \
//								subcontrol-position: top left; \
//								padding: 3px 3px; \
//							  }")
//
//#define CSS_CUSTOM_QLABEL QString(" \
//						   QLabel \
//						   { \
//							 color: #FFFFFF; \
//							 font: bold, 16px; \
//							 background: transparent; \
//						   }")
//
//#define CSS_CUSTOM_QSPINBOX QString(" \
//							 QSpinBox \
//							 { \
//							   background-color: transparent; \
//							   border: 1px solid #FFFFFF; \
//							   border-radius: 4px; \
//							   color: white; \
//							   font: bold; \
//							 }")
//
//#define CSS_CUSTOM_QDOUBLESPINBOX QString(" \
//							 QDoubleSpinBox \
//							 { \
//							   background-color: transparent; \
//							   border: 1px solid #FFFFFF; \
//							   border-radius: 4px; \
//							   color: white; \
//							   font: bold; \
//							 }")
//
//#define CSS_CUSTOM_QCOMBOBOX QString(" \
//							  QComboBox QAbstractItemView \
//							  { \
//								background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #00CC99, stop: 1 #000000); \
//								color: #FFFFFF; \
//								selection-background-color: #00CC99; \
//							  }")
//
//#define CSS_CUSTOM_QCHECKBOX QString(" \
//							  QCheckBox \
//							  { \
//								color: #FFFFFF; \
//								font: bold, 16px; \
//							  }")
//
//#define CSS_CUSTOM_QTEXTEDIT QString(" \
//							  QTextEdit \
//							  { \
//								background: transparent; \
//								border: 1px solid transparent; \
//								color: #FFFFFF; \
//							  }")
//
//#define CSS_CUSTOM_QTREEVIEW QString(" \
//							  QTreeView { \
//								background-color: transparent; \
//								border: 1px solid transparent; \
//							  } \
//							  QTreeView::item { \
//								background: transparent; \
//								color: #FFFFFF; \
//						      } \
//							  ")
//
//#define CSS_CUSTOM_QTABLEWIDGET QString(" \
//							  QTableWidget { \
//								background-color: transparent; \
//								border: 1px solid transparent; \
//							  } \
//							  QTableWidget::item { \
//								color: #FFFFFF; \
//						      } \
//							  QTableView QTableCornerButton::section { \
//									background: transparent; \
//							  }")
//
//#define CSS_CUSTOM_QHEADERVIEW QString(" \
//							  QHeaderView { \
//								background: transparent; \
//							  } \
//							  QHeaderView::section { \
//								background: transparent; \
//								color: #FFFFFF; \
//								border: 1px solid transparent; \
//							  }")
//
//#define CSS_CUSTOM_QSCROLLAREA QString(" \
//							  QScrollArea { \
//								background: transparent; \
//								border: 1px solid transparent; \
//							  } \
//							  QScrollArea > QWidget > QWidget { \
//								background: transparent; \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QWTPLOT QString(" \
//							  QwtPlotCanvas { \
//								background: transparent; \
//							  } \
//							 QwtScaleWidget { \
//								color: #FFFFFF; \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QTOOLBAR QString(" \
//							  QToolBar { \
//								background: transparent; \
//								border: 1px solid transparent; \
//								color: #FFFFFF; \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QINPUTDIALOG QString(" \
//							  QInputDialog { \
//								background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #00CC99, stop: 1 #000000); \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QMESSAGEBOX QString(" \
//							  QMessageBox { \
//								background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #00CC99, stop: 1 #000000); \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QSTATUSBAR QString(" \
//							  QStatusBar { \
//								color: #FFFFFF; \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QGRAPHICSVIEW QString(" \
//							  QGraphicsView { \
//								background: transparent; \
//								border: 1px solid transparent; \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QLINEEDIT QString(" \
//							 QLineEdit \
//							 { \
//							   background-color: transparent; \
//							   border: 1px solid #FFFFFF; \
//							   border-radius: 4px; \
//							   color: #FFFFFF; \
//							   font: bold; \
//							 }")
//
//#define CSS_CUSTOM_QRADIOBUTTON QString(" \
//							  QRadioButton \
//							  { \
//								color: #FFFFFF; \
//								font: bold, 16px; \
//							  }")

//#define CSS_CUSTOM_QPUSHBUTTON QString(" \
//							  QPushButton \
//							  { \
//								border: 2px solid #FFFFFF; \
//								border-radius: 4px; \
//								background-color: transparent; \
//								color: #FFFFFF; \
//								font: bold, 20px; \
//								min-height: 25px; \
//								min-width: 80px; \
//							  } \
//							  QPushButton:pressed { \
//							    background-color: #FFFFFF; \
//								color: #00664C; \
//							  } \
//							 }")


#define CSS_MAIN_COLOUR QString("#3B3B3B")
#define CSS_DARKEST_COLOUR QString("#262626")
#define CSS_TEXT_COLOUR QString("#FFFFFF")
#define CSS_TEXT_DISABLED_COLOUR QString("#8A8A8A")
#define CSS_SELECTED_COLOUR QString("#8A8A8A")
#define CSS_PRESSED_COLOUR QString("#000000")

#define CSS_CUSTOM_QMAINWINDOW QString(" \
								QMainWindow \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								}")

#define CSS_CUSTOM_QMANUBAR QString(" \
							 QMenuBar \
							 { \
								background-color: " + CSS_MAIN_COLOUR + "; \
								color: " + CSS_TEXT_COLOUR + "; \
							 } \
						     QMenuBar::item \
							 { \
								spacing: 3px; \
								padding: 1px 4px; \
								background:  " + CSS_MAIN_COLOUR + "; \
								border-radius: 2px; \
							 } \
							 QMenuBar::item:selected \
							 { \
								background: " + CSS_SELECTED_COLOUR + "; \
							 } \
							 QMenuBar::item:pressed { \
								background: " + CSS_SELECTED_COLOUR + "; \
								color: " + CSS_TEXT_COLOUR + "; \
							 }")

#define CSS_CUSTOM_QMENU QString(" \
						  QMenu \
						  { \
							background-color: " + CSS_MAIN_COLOUR + ";\
						  } \
						  QMenu::item::selected \
						  { \
							 background-color: " + CSS_SELECTED_COLOUR + "; \
							 color: " + CSS_TEXT_COLOUR + "; \
						  } \
						  QMenu::item::!selected \
						  { \
							background-color: " + CSS_MAIN_COLOUR + "; \
							color: " + CSS_TEXT_COLOUR + "; \
						  } \
						  QMenu::item::!enabled \
						  { \
							background-color: " + CSS_MAIN_COLOUR + "; \
							color: " + CSS_TEXT_DISABLED_COLOUR + "; \
						  }")

#define CSS_CUSTOM_QTABWIDGET QString(" \
							   QTabWidget::pane \
							   { \
								 border: 1px solid " + CSS_DARKEST_COLOUR + "; \
								 border-top-color: " + CSS_SELECTED_COLOUR + "; \
								 background: " + CSS_DARKEST_COLOUR + "; \
							   } \
							   QTabBar::tab \
							   { \
							 	 background: " + CSS_MAIN_COLOUR + "; \
								 border: 1px solid " + CSS_MAIN_COLOUR + "; \
								 border-top-color: " + CSS_MAIN_COLOUR + "; \
								 border-bottom-color: " + CSS_MAIN_COLOUR + "; \
								 border-top-left-radius: 0px; \
								 border-top-right-radius: 0px; \
				  				 padding: 2px; \
								 color: " + CSS_TEXT_COLOUR + "; \
							   } \
							   QTabBar::tab:hover \
							   { \
								 background: " + CSS_SELECTED_COLOUR + "; \
							   } \
							   QTabBar::tab:selected \
							   { \
								 border: 1px solid " + CSS_SELECTED_COLOUR + "; \
								 background: " + CSS_SELECTED_COLOUR + "; \
							   } \
							   QTabBar::tab:!selected \
							   { \
								 background: " + CSS_MAIN_COLOUR + "; \
								 margin-top: 0px; \
							   }")

#define CSS_CUSTOM_QGROUPBOX QString(" \
							  QGroupBox \
							  { \
								border: 1px solid " + CSS_TEXT_COLOUR + "; \
								border-radius: 2px; \
								margin-top: 10px; \
							    color: " + CSS_TEXT_COLOUR + "; \
							  } \
							  QGroupBox::title \
							  { \
								subcontrol-origin: margin; \
								subcontrol-position: top left; \
								padding: 0px 10px; \
							  } \
							  QGroupBox::indicator { \
								border-radius: 4px; \
								width: 15px; \
								height: 15px; \
							  } \
							  QGroupBox::indicator:checked { \
								width: 15px; \
								height: 15px; \
								image: url(:/files/checkBox_checked_icon.png); \
							  } \
							  QGroupBox::indicator:unchecked { \
								border: 1px solid " + CSS_TEXT_COLOUR + "; \
							  } \
							  QGroupBox::indicator:!enabled { \
								border: 1px solid " + CSS_MAIN_COLOUR + "; \
							  }")

#define CSS_CUSTOM_QLABEL QString(" \
						   QLabel \
						   { \
							 color: " + CSS_TEXT_COLOUR + "; \
							 font: bold, 16px; \
							 background: transparent; \
						   }")

#define CSS_CUSTOM_QSPINBOX QString(" \
							 QSpinBox \
							 { \
							   background-color: transparent; \
							   border: 1px solid " + CSS_TEXT_COLOUR + "; \
							   border-radius: 2px; \
							   color: " + CSS_TEXT_COLOUR + "; \
							   font: bold; \
							 } \
						     QSpinBox:!enabled \
							 { \
							   border: 1px solid " + CSS_SELECTED_COLOUR + "; \
							   color: " + CSS_SELECTED_COLOUR + "; \
							 }")

#define CSS_CUSTOM_QDOUBLESPINBOX QString(" \
							 QDoubleSpinBox \
							 { \
							   background-color: transparent; \
							   border: 1px solid " + CSS_TEXT_COLOUR + "; \
							   border-radius: 2px; \
							   color: " + CSS_TEXT_COLOUR + "; \
							   font: bold; \
							 } \
							 QDoubleSpinBox:!enabled \
							 { \
							   border: 1px solid " + CSS_SELECTED_COLOUR + "; \
							   color: " + CSS_SELECTED_COLOUR + "; \
							 }")

#define CSS_CUSTOM_QCOMBOBOX QString(" \
							  QComboBox { \
								background-color: transparent; \
								border: 1px solid " + CSS_TEXT_COLOUR + "; \
								border-radius: 2px; \
								padding: 1px 18px 1px 3px; \
							    color: " + CSS_TEXT_COLOUR + "; \
							  } \
							  QComboBox::drop-down \
							  { \
								border: 1px solid transparent; \
							  } \
							  QComboBox::down-arrow { \
								width: 10px; \
								height: 10px; \
								image: url(:/files/comboBox_arrow_icon.png); \
							  } \
							  QComboBox QAbstractItemView \
							  { \
								border: 1px solid " + CSS_MAIN_COLOUR + "; \
								background: " + CSS_MAIN_COLOUR + "; \
								color: " + CSS_TEXT_COLOUR + "; \
								selection-background-color: " + CSS_SELECTED_COLOUR + "; \
							  }")

#define CSS_CUSTOM_QCHECKBOX QString(" \
							  QCheckBox \
							  { \
								background-color: transparent; \
								color: " + CSS_TEXT_COLOUR + "; \
								font: bold, 16px; \
							  } \
							  QCheckBox::indicator { \
								border-radius: 4px; \
								width: 15px; \
								height: 15px; \
							  } \
							  QCheckBox::indicator:checked { \
								width: 15px; \
								height: 15px; \
								image: url(:/files/checkBox_checked_icon.png); \
							  } \
							  QCheckBox::indicator:unchecked { \
								border: 1px solid " + CSS_TEXT_COLOUR + "; \
							  } \
							  QCheckBox::indicator:!enabled { \
								border: 1px solid " + CSS_MAIN_COLOUR + "; \
							  }")

#define CSS_CUSTOM_QTEXTEDIT QString(" \
							  QTextEdit \
							  { \
								background: transparent; \
								border: 1px solid transparent; \
								color: " + CSS_TEXT_COLOUR + "; \
							  }")

#define CSS_CUSTOM_QTREEVIEW QString(" \
							  QTreeView { \
								background-color: " + CSS_DARKEST_COLOUR + "; \
								border: 1px solid " + CSS_DARKEST_COLOUR + "; \
							  } \
							  QTreeView::item { \
								background: " + CSS_DARKEST_COLOUR + "; \
								color: " + CSS_TEXT_COLOUR + "; \
						      } \
							  QTreeView::item:hover { \
								background: " + CSS_SELECTED_COLOUR + "; \
								border: 1px solid " + CSS_SELECTED_COLOUR + "; \
							  }")

#define CSS_CUSTOM_QTABLEWIDGET QString(" \
							  QTableWidget { \
								background-color: transparent; \
								border: 1px solid transparent; \
							  } \
							  QTableWidget::item { \
								border: 1px solid transparent; \
								color: " + CSS_TEXT_COLOUR + "; \
						      } \
							  QTableView QTableCornerButton::section { \
								background: transparent; \
							  }")

#define CSS_CUSTOM_QHEADERVIEW QString(" \
							  QHeaderView { \
								background: transparent; \
							  } \
							  QHeaderView::section { \
								background: transparent; \
								color: " + CSS_TEXT_COLOUR + "; \
								border: 1px solid transparent; \
							  }")

#define CSS_CUSTOM_QSCROLLAREA QString(" \
							  QScrollArea { \
								background: transparent; \
								border: 1px solid transparent; \
							  } \
							  QScrollArea > QWidget > QWidget { \
								background: transparent; \
							  } \
							  ")

#define CSS_CUSTOM_QWTPLOT QString(" \
							  QwtPlotCanvas { \
								background: transparent; \
							  } \
							  QwtScaleWidget { \
								color: " + CSS_TEXT_COLOUR + "; \
							  } \
							  ")

#define CSS_CUSTOM_QTOOLBAR QString(" \
							  QToolBar { \
								background: transparent; \
								border: 1px solid transparent; \
								color: " + CSS_TEXT_COLOUR + "; \
							  } \
							  ")

#define CSS_CUSTOM_QINPUTDIALOG QString(" \
							  QInputDialog { \
								background-color: " + CSS_MAIN_COLOUR + "; \
							  } \
							  ")

#define CSS_CUSTOM_QMESSAGEBOX QString(" \
							  QMessageBox { \
								background-color: " + CSS_MAIN_COLOUR + "; \
							  } \
							  ")

#define CSS_CUSTOM_QSTATUSBAR QString(" \
							  QStatusBar { \
								background-color: " + CSS_MAIN_COLOUR + "; \
								color: " + CSS_TEXT_COLOUR + "; \
							  } \
							  ")

#define CSS_CUSTOM_QGRAPHICSVIEW QString(" \
							  QGraphicsView { \
								background: transparent; \
								border: 1px solid transparent; \
							  } \
							  ")

#define CSS_CUSTOM_QLINEEDIT QString(" \
							 QLineEdit \
							 { \
							   background-color: transparent; \
							   border: 1px solid " + CSS_TEXT_COLOUR + "; \
							   border-radius: 2px; \
							   color: " + CSS_TEXT_COLOUR + "; \
							   font: bold; \
							 }")

#define CSS_CUSTOM_QRADIOBUTTON QString(" \
							  QRadioButton \
							  { \
								color: " + CSS_TEXT_COLOUR + "; \
								font: bold, 16px; \
							  } \
							  QRadioButton::indicator { \
								border: 2px solid transparent; \
								width: 15px; \
								height: 15px; \
							  } \
							  QRadioButton::indicator:checked { \
								width: 20px; \
								height: 20px; \
								image: url(:/files/radioButton_checked_icon.png); \
							  } \
							  QRadioButton::indicator:unchecked { \
								width: 20px; \
								height: 20px; \
								image: url(:/files/radioButton_unchecked_icon.png); \
							  } \
							  QRadioButton::indicator:!enabled { \
								border-radius: 15px; \
								border: 2px solid " + CSS_SELECTED_COLOUR + "; \
							  }")

#define CSS_CUSTOM_QPUSHBUTTON QString(" \
							  QPushButton \
							  { \
								border: 1px solid " + CSS_TEXT_COLOUR + "; \
								border-radius: 2px; \
								background-color: transparent; \
								color: " + CSS_TEXT_COLOUR + "; \
								font: bold; \
								padding: 5 5 5 5px; \
							  } \
							  QPushButton:pressed { \
							    background-color: " + CSS_TEXT_COLOUR + "; \
								color: " + CSS_SELECTED_COLOUR + "; \
							  } \
							  QPushButton:!enabled { \
							    border: 1px solid " + CSS_SELECTED_COLOUR + "; \
								color: " + CSS_SELECTED_COLOUR + "; \
							  }")

#define CSS_CUSTOM_QTOOLBUTTON QString(" \
							  QToolButton { \
								border: 2px solid transparent; \
								border-radius: 2px; \
								background-color: transparent; \
								font: bold; \
								color: " + CSS_TEXT_COLOUR + "; \
							  } \
							  QToolButton:!enabled { \
								color: " + CSS_SELECTED_COLOUR + "; \
							  } \
							  QToolButton:checked { \
								border: 1px solid " + CSS_TEXT_COLOUR + "; \
							  }")

#define CSS_CUSTOM_STYLESHEET CSS_CUSTOM_QMAINWINDOW + CSS_CUSTOM_QMANUBAR + \
								CSS_CUSTOM_QMENU + CSS_CUSTOM_QTABWIDGET + CSS_CUSTOM_QGROUPBOX + \
								CSS_CUSTOM_QLABEL + CSS_CUSTOM_QSPINBOX + CSS_CUSTOM_QDOUBLESPINBOX + \
								CSS_CUSTOM_QCOMBOBOX + CSS_CUSTOM_QCHECKBOX + \
								CSS_CUSTOM_QTEXTEDIT + CSS_CUSTOM_QTREEVIEW + CSS_CUSTOM_QTABLEWIDGET + \
								CSS_CUSTOM_QHEADERVIEW + CSS_CUSTOM_QSCROLLAREA + CSS_CUSTOM_QWTPLOT + \
								CSS_CUSTOM_QTOOLBAR + CSS_CUSTOM_QINPUTDIALOG + CSS_CUSTOM_QMESSAGEBOX + \
								CSS_CUSTOM_QSTATUSBAR + CSS_CUSTOM_QGRAPHICSVIEW + CSS_CUSTOM_QLINEEDIT + \
								CSS_CUSTOM_QRADIOBUTTON + CSS_CUSTOM_QPUSHBUTTON + CSS_CUSTOM_QTOOLBUTTON

#define CSS_CUSTOM_QWIDGET QString(" \
								QWidget#CCMTuning \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								} \
							    QWidget#CCMConfig \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								} \
								QWidget#CCMConfigAdv \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								} \
							    QWidget#LSHTuning \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								} \
							    QWidget#LSHConfig \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								} \
								QWidget#LSHConfigAdv \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								} \
							    QWidget#LSHGridView \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								} \
							    QDialog \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								} \
								QWidget#LCATuning \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								} \
								QWidget#LCAConfig \
								{ \
									background-color: " + CSS_MAIN_COLOUR + "; \
								}")

#define CSS_CUSTOM_VT_STYLESHEET CSS_CUSTOM_QWIDGET + CSS_CUSTOM_STYLESHEET

//#define CSS_QGROUPBOX_SELECTED QString("QGroupBox { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 " + CSS_DARKEST_COLOUR + "); }")
#define CSS_QGROUPBOX_SELECTED QString("QGroupBox { background-color: rgba(0, 255, 0, 0.3); }")
//#define CSS_QWIDGET_CHANGED QString("QWidget#" + sender()->objectName() + " { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 #FF0000); color: #FFFFFF; }")
#define CSS_QWIDGET_CHANGED QString("QWidget#" + sender()->objectName() + " { background-color: rgba(255, 0, 0, 1.0); color: #FFFFFF; }")

} // namespace VisionLive

#endif // CSS_H
