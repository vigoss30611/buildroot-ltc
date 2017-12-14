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


//#define CSS_MAIN_COLOUR QString("#3B3B3B")
//#define CSS_DARKEST_COLOUR QString("#262626")
//#define CSS_TEXT_COLOUR QString("#FFFFFF")
//#define CSS_TEXT_DISABLED_COLOUR QString("#8A8A8A")
//#define CSS_SELECTED_COLOUR QString("#8A8A8A")
//#define CSS_PRESSED_COLOUR QString("#000000")
//#define CSS_BUTTON_HOVER_COLOUR QString("#B3B3B3")
//
//#define CSS_CUSTOM_QMAINWINDOW QString(" \
//								QMainWindow \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								}")
//
//#define CSS_CUSTOM_QMANUBAR QString(" \
//							 QMenuBar \
//							 { \
//								background-color: " + CSS_MAIN_COLOUR + "; \
//								color: " + CSS_TEXT_COLOUR + "; \
//							 } \
//						     QMenuBar::item \
//							 { \
//								spacing: 3px; \
//								padding: 1px 4px; \
//								background:  " + CSS_MAIN_COLOUR + "; \
//								border-radius: 2px; \
//							 } \
//							 QMenuBar::item:selected \
//							 { \
//								background: " + CSS_SELECTED_COLOUR + "; \
//							 } \
//							 QMenuBar::item:pressed { \
//								background: " + CSS_SELECTED_COLOUR + "; \
//								color: " + CSS_TEXT_COLOUR + "; \
//							 }")
//
//#define CSS_CUSTOM_QMENU QString(" \
//						  QMenu \
//						  { \
//							background-color: " + CSS_MAIN_COLOUR + ";\
//						  } \
//						  QMenu::item::selected \
//						  { \
//							 background-color: " + CSS_SELECTED_COLOUR + "; \
//							 color: " + CSS_TEXT_COLOUR + "; \
//						  } \
//						  QMenu::item::!selected \
//						  { \
//							background-color: " + CSS_MAIN_COLOUR + "; \
//							color: " + CSS_TEXT_COLOUR + "; \
//						  } \
//						  QMenu::item::!enabled \
//						  { \
//							background-color: " + CSS_MAIN_COLOUR + "; \
//							color: " + CSS_TEXT_DISABLED_COLOUR + "; \
//						  }")
//
//#define CSS_CUSTOM_QTABWIDGET QString(" \
//							   QTabWidget::pane \
//							   { \
//								 border: 1px solid " + CSS_DARKEST_COLOUR + "; \
//								 border-top-color: " + CSS_SELECTED_COLOUR + "; \
//								 background: " + CSS_DARKEST_COLOUR + "; \
//							   } \
//							   QTabBar::tab \
//							   { \
//							 	 background: " + CSS_MAIN_COLOUR + "; \
//								 border: 1px solid " + CSS_MAIN_COLOUR + "; \
//								 border-top-color: " + CSS_MAIN_COLOUR + "; \
//								 border-bottom-color: " + CSS_MAIN_COLOUR + "; \
//								 border-top-left-radius: 0px; \
//								 border-top-right-radius: 0px; \
//				  				 padding: 2px; \
//								 color: " + CSS_TEXT_COLOUR + "; \
//							   } \
//							   QTabBar::tab:hover \
//							   { \
//								 background: " + CSS_SELECTED_COLOUR + "; \
//							   } \
//							   QTabBar::tab:selected \
//							   { \
//								 border: 1px solid " + CSS_SELECTED_COLOUR + "; \
//								 background: " + CSS_SELECTED_COLOUR + "; \
//							   } \
//							   QTabBar::tab:!selected \
//							   { \
//								 background: " + CSS_MAIN_COLOUR + "; \
//								 margin-top: 0px; \
//							   }")
//
//#define CSS_CUSTOM_QGROUPBOX QString(" \
//							  QGroupBox \
//							  { \
//								border: 1px solid " + CSS_TEXT_COLOUR + "; \
//								border-radius: 2px; \
//								margin-top: 10px; \
//							    color: " + CSS_TEXT_COLOUR + "; \
//							  } \
//							  QGroupBox::title \
//							  { \
//								subcontrol-origin: margin; \
//								subcontrol-position: top left; \
//								padding: 0px 10px; \
//							  } \
//							  QGroupBox::indicator { \
//								border-radius: 4px; \
//								width: 15px; \
//								height: 15px; \
//							  } \
//							  QGroupBox::indicator:checked { \
//								width: 15px; \
//								height: 15px; \
//								image: url(:/files/checkBox_checked_icon.png); \
//							  } \
//							  QGroupBox::indicator:unchecked { \
//								border: 1px solid " + CSS_TEXT_COLOUR + "; \
//							  } \
//							  QGroupBox::indicator:!enabled { \
//								border: 1px solid " + CSS_MAIN_COLOUR + "; \
//							  }")
//
//#define CSS_CUSTOM_QLABEL QString(" \
//						   QLabel \
//						   { \
//							 color: " + CSS_TEXT_COLOUR + "; \
//							 font: bold, 16px; \
//							 background: transparent; \
//						   }")
//
//#define CSS_CUSTOM_QSPINBOX QString(" \
//							 QSpinBox \
//							 { \
//							   background-color: transparent; \
//							   border: 1px solid " + CSS_TEXT_COLOUR + "; \
//							   border-radius: 2px; \
//							   color: " + CSS_TEXT_COLOUR + "; \
//							   font: bold; \
//							 } \
//						     QSpinBox:!enabled \
//							 { \
//							   border: 1px solid " + CSS_SELECTED_COLOUR + "; \
//							   color: " + CSS_SELECTED_COLOUR + "; \
//							 }")
//
//#define CSS_CUSTOM_QDOUBLESPINBOX QString(" \
//							 QDoubleSpinBox \
//							 { \
//							   background-color: transparent; \
//							   border: 1px solid " + CSS_TEXT_COLOUR + "; \
//							   border-radius: 2px; \
//							   color: " + CSS_TEXT_COLOUR + "; \
//							   font: bold; \
//							 } \
//							 QDoubleSpinBox:!enabled \
//							 { \
//							   border: 1px solid " + CSS_SELECTED_COLOUR + "; \
//							   color: " + CSS_SELECTED_COLOUR + "; \
//							 }")
//
//#define CSS_CUSTOM_QCOMBOBOX QString(" \
//							  QComboBox { \
//								background-color: transparent; \
//								border: 1px solid " + CSS_TEXT_COLOUR + "; \
//								border-radius: 2px; \
//								padding: 1px 18px 1px 3px; \
//							    color: " + CSS_TEXT_COLOUR + "; \
//							  } \
//							  QComboBox::drop-down \
//							  { \
//								border: 1px solid transparent; \
//							  } \
//							  QComboBox::down-arrow { \
//								width: 10px; \
//								height: 10px; \
//								image: url(:/files/comboBox_arrow_icon.png); \
//							  } \
//							  QComboBox QAbstractItemView \
//							  { \
//								border: 1px solid " + CSS_MAIN_COLOUR + "; \
//								background: " + CSS_MAIN_COLOUR + "; \
//								color: " + CSS_TEXT_COLOUR + "; \
//								selection-background-color: " + CSS_SELECTED_COLOUR + "; \
//							  }")
//
//#define CSS_CUSTOM_QCHECKBOX QString(" \
//							  QCheckBox \
//							  { \
//								background-color: transparent; \
//								color: " + CSS_TEXT_COLOUR + "; \
//								font: bold, 16px; \
//							  } \
//							  QCheckBox::indicator { \
//								border-radius: 4px; \
//								width: 15px; \
//								height: 15px; \
//							  } \
//							  QCheckBox::indicator:checked { \
//								width: 15px; \
//								height: 15px; \
//								image: url(:/files/checkBox_checked_icon.png); \
//							  } \
//							  QCheckBox::indicator:unchecked { \
//								border: 1px solid " + CSS_TEXT_COLOUR + "; \
//							  } \
//							  QCheckBox::indicator:!enabled { \
//								border: 1px solid " + CSS_MAIN_COLOUR + "; \
//							  }")
//
//#define CSS_CUSTOM_QTEXTEDIT QString(" \
//							  QTextEdit \
//							  { \
//								background: transparent; \
//								border: 1px solid transparent; \
//								color: " + CSS_TEXT_COLOUR + "; \
//							  }")
//
//#define CSS_CUSTOM_QTREEVIEW QString(" \
//							  QTreeView { \
//								background-color: " + CSS_DARKEST_COLOUR + "; \
//								border: 1px solid " + CSS_DARKEST_COLOUR + "; \
//							  } \
//							  QTreeView::item { \
//								background: " + CSS_DARKEST_COLOUR + "; \
//								color: " + CSS_TEXT_COLOUR + "; \
//						      } \
//							  QTreeView::item:hover { \
//								background: " + CSS_SELECTED_COLOUR + "; \
//								border: 1px solid " + CSS_SELECTED_COLOUR + "; \
//							  }")
//
//#define CSS_CUSTOM_QTABLEWIDGET QString(" \
//							  QTableWidget { \
//								background-color: transparent; \
//								border: 1px solid transparent; \
//							  } \
//							  QTableWidget::item { \
//								border: 1px solid transparent; \
//								color: " + CSS_TEXT_COLOUR + "; \
//						      } \
//							  QTableView QTableCornerButton::section { \
//								background: transparent; \
//							  }")
//
//#define CSS_CUSTOM_QHEADERVIEW QString(" \
//							  QHeaderView { \
//								background: transparent; \
//							  } \
//							  QHeaderView::section { \
//								background: transparent; \
//								color: " + CSS_TEXT_COLOUR + "; \
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
//							  QwtScaleWidget { \
//								color: " + CSS_TEXT_COLOUR + "; \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QTOOLBAR QString(" \
//							  QToolBar { \
//								background: transparent; \
//								border: 1px solid transparent; \
//								color: " + CSS_TEXT_COLOUR + "; \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QINPUTDIALOG QString(" \
//							  QInputDialog { \
//								background-color: " + CSS_MAIN_COLOUR + "; \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QMESSAGEBOX QString(" \
//							  QMessageBox { \
//								background-color: " + CSS_MAIN_COLOUR + "; \
//							  } \
//							  ")
//
//#define CSS_CUSTOM_QSTATUSBAR QString(" \
//							  QStatusBar { \
//								background-color: " + CSS_MAIN_COLOUR + "; \
//								color: " + CSS_TEXT_COLOUR + "; \
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
//							   border: 1px solid " + CSS_TEXT_COLOUR + "; \
//							   border-radius: 2px; \
//							   color: " + CSS_TEXT_COLOUR + "; \
//							   font: bold; \
//							 }")
//
//#define CSS_CUSTOM_QRADIOBUTTON QString(" \
//							  QRadioButton \
//							  { \
//								color: " + CSS_TEXT_COLOUR + "; \
//								font: bold, 16px; \
//							  } \
//							  QRadioButton::indicator { \
//								border: 2px solid transparent; \
//								width: 15px; \
//								height: 15px; \
//							  } \
//							  QRadioButton::indicator:checked { \
//								width: 20px; \
//								height: 20px; \
//								image: url(:/files/radioButton_checked_icon.png); \
//							  } \
//							  QRadioButton::indicator:unchecked { \
//								width: 20px; \
//								height: 20px; \
//								image: url(:/files/radioButton_unchecked_icon.png); \
//							  } \
//							  QRadioButton::indicator:!enabled { \
//								border-radius: 15px; \
//								border: 2px solid " + CSS_SELECTED_COLOUR + "; \
//							  }")
//
//#define CSS_CUSTOM_QPUSHBUTTON QString(" \
//							  QPushButton \
//							  { \
//								border: 1px solid " + CSS_TEXT_COLOUR + "; \
//								border-radius: 2px; \
//								background-color: transparent; \
//								color: " + CSS_TEXT_COLOUR + "; \
//								font: bold; \
//								padding: 5 5 5 5px; \
//							  } \
//							  QPushButton:pressed { \
//							    background-color: " + CSS_TEXT_COLOUR + "; \
//								color: " + CSS_SELECTED_COLOUR + "; \
//							  } \
//							  QPushButton:!enabled { \
//							    border: 1px solid " + CSS_SELECTED_COLOUR + "; \
//								color: " + CSS_SELECTED_COLOUR + "; \
//							  } \
//							  QPushButton:hover { \
//							    border: 1px solid " + CSS_BUTTON_HOVER_COLOUR + "; \
//								color: " + CSS_SELECTED_COLOUR + "; \
//							  }")
//
//#define CSS_CUSTOM_QPUSHBUTTON2 QString(" \
//QPushButton \
//{ \
//    color: silver; \
//    background-color: #302F2F; \
//    border-width: 1px; \
//    border-color: #4A4949; \
//    border-style: solid; \
//    padding-top: 5px; \
//    padding-bottom: 5px; \
//    padding-left: 5px; \
//    padding-right: 5px; \
//    border-radius: 2px; \
//    outline: none; \
//} \
// \
//QPushButton:disabled \
//{ \
//    background-color: #302F2F; \
//    border-width: 1px; \
//    border-color: #3A3939; \
//    border-style: solid; \
//    padding-top: 5px; \
//    padding-bottom: 5px; \
//    padding-left: 10px; \
//    padding-right: 10px; \
//    /*border-radius: 2px;*/ \
//    color: #454545; \
//} \
// \
//QPushButton:checked{ \
//    background-color: #4A4949; \
//    border-color: #6A6969; \
//} \
// \
//QPushButton:pressed \
//{ \
//    background-color: #484846; \
//} \
// \
//QPushButton:hover \
//{ \
//    border: 1px solid #78879b; \
//    color: silver; \
//} \
// \
//QPushButton::menu-indicator  { \
//    subcontrol-origin: padding; \
//    subcontrol-position: bottom right; \
//    left: 8px; \
//} \
//")
//
//#define CSS_CUSTOM_QTOOLBUTTON QString(" \
//							  QToolButton { \
//								border: 2px solid transparent; \
//								border-radius: 2px; \
//								background-color: transparent; \
//								font: bold; \
//								color: " + CSS_TEXT_COLOUR + "; \
//							  } \
//							  QToolButton:!enabled { \
//								color: " + CSS_SELECTED_COLOUR + "; \
//							  } \
//							  QToolButton:checked { \
//								border: 1px solid " + CSS_TEXT_COLOUR + "; \
//							  }")
//
//#define CSS_CUSTOM_QSLIDER QString(" \
//							QSlider::groove:horizontal { \
//								border: 1px solid #3A3939; \
//								height: 8px; \
//								background: #201F1F; \
//								margin: 2px 0; \
//								border-radius: 2px; \
//							} \
//							 \
//							QSlider::handle:horizontal { \
//								background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, \
//								  stop: 0.0 silver, stop: 0.2 #a8a8a8, stop: 1 #727272); \
//								border: 1px solid #3A3939; \
//								width: 14px; \
//								height: 14px; \
//								margin: -4px 0; \
//								border-radius: 2px; \
//							} \
//							 \
//							QSlider::groove:vertical { \
//								border: 1px solid #3A3939; \
//								width: 8px; \
//								background: #201F1F; \
//								margin: 0 0px; \
//								border-radius: 2px; \
//							} \
//							 \
//							QSlider::handle:vertical { \
//								background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0.0 silver, \
//								  stop: 0.2 #a8a8a8, stop: 1 #727272); \
//								border: 1px solid #3A3939; \
//								width: 14px; \
//								height: 14px; \
//								margin: 0 -4px; \
//								border-radius: 2px; \
//							}")
//
//#define CSS_CUSTOM_QSCROLLBAR QString(" \
//							QScrollBar:horizontal \
//							{ \
//								height: 15px; \
//								margin: 3px 15px 3px 15px; \
//								border: 1px solid #3A3939; \
//								border-radius: 4px; \
//								background-color: #201F1F; \
//							} \
//							 \
//							QScrollBar::handle:horizontal \
//							{ \
//								background-color: #605F5F; \
//								min-width: 25px; \
//								border-radius: 4px; \
//							} \
//							 \
//							QScrollBar::add-line:horizontal \
//							{ \
//								background: none; \
//								margin: 0px 3px 0px 3px; \
//								width: 10px; \
//								height: 10px; \
//								subcontrol-position: right; \
//								subcontrol-origin: margin; \
//							} \
//							 \
//							QScrollBar::sub-line:horizontal \
//							{ \
//								background: none; \
//								margin: 0px 3px 0px 3px; \
//								height: 10px; \
//								width: 10px; \
//								subcontrol-position: left; \
//								subcontrol-origin: margin; \
//							} \
//							 \
//							QScrollBar::add-line:horizontal:hover,QScrollBar::add-line:horizontal:on \
//							{ \
//								background: none; \
//								height: 10px; \
//								width: 10px; \
//								subcontrol-position: right; \
//								subcontrol-origin: margin; \
//							} \
//							 \
//							 \
//							QScrollBar::sub-line:horizontal:hover, QScrollBar::sub-line:horizontal:on \
//							{ \
//								background: none; \
//								height: 10px; \
//								width: 10px; \
//								subcontrol-position: left; \
//								subcontrol-origin: margin; \
//							} \
//							 \
//							QScrollBar::up-arrow:horizontal, QScrollBar::down-arrow:horizontal \
//							{ \
//								background: none; \
//							} \
//							 \
//							 \
//							QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal \
//							{ \
//								background: none; \
//							} \
//							 \
//							QScrollBar:vertical \
//							{ \
//								width: 15px; \
//								margin: 15px 3px 15px 3px; \
//								border: 1px solid #3A3939; \
//								border-radius: 4px; \
//								background-color: #201F1F; \
//							} \
//							 \
//							QScrollBar::handle:vertical \
//							{ \
//								background-color: #605F5F; \
//								min-height: 25px; \
//								border-radius: 4px; \
//							} \
//							 \
//							QScrollBar::sub-line:vertical \
//							{ \
//								background: none; \
//								margin: 3px 0px 3px 0px; \
//								height: 10px; \
//								width: 10px; \
//								subcontrol-position: top; \
//								subcontrol-origin: margin; \
//							} \
//							 \
//							QScrollBar::add-line:vertical \
//							{ \
//								background: none; \
//								margin: 3px 0px 3px 0px; \
//								height: 10px; \
//								width: 10px; \
//								subcontrol-position: bottom; \
//								subcontrol-origin: margin; \
//							} \
//							 \
//							QScrollBar::sub-line:vertical:hover,QScrollBar::sub-line:vertical:on \
//							{ \
//							 \
//								background: none; \
//								height: 10px; \
//								width: 10px; \
//								subcontrol-position: top; \
//								subcontrol-origin: margin; \
//							} \
//							 \
//							 \
//							QScrollBar::add-line:vertical:hover, QScrollBar::add-line:vertical:on \
//							{ \
//								background: none; \
//								height: 10px; \
//								width: 10px; \
//								subcontrol-position: bottom; \
//								subcontrol-origin: margin; \
//							} \
//							 \
//							QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical \
//							{ \
//								background: none; \
//							} \
//							 \
//							 \
//							QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical \
//							{ \
//								background: none; \
//							}")
//
//#define CSS_CUSTOM_DARKORANGE QString(" \
//QWidget \
//{ \
//    color: #b1b1b1; \
//    background-color: #323232; \
//} \
// \
//QWidget:item:hover \
//{ \
//    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #ca0619); \
//    color: #000000; \
//} \
// \
//QWidget:item:selected \
//{ \
//    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
//} \
//QLabel \
//{ \
//	background: transparent; \
//} \
//QToolTip \
//{ \
//     border: 1px solid black; \
//     background-color: #ffa02f; \
//     padding: 1px; \
//     border-radius: 3px; \
//     opacity: 1000; \
//} \
//QToolBar { \
//	background: transparent; \
//	border: 1px solid transparent; \
//} \
// \
//QMenuBar::item \
//{ \
//    background: transparent; \
//} \
// \
//QMenuBar::item:selected \
//{ \
//    border: 1px solid #ffaa00; \
//} \
// \
//QMenuBar::item:pressed \
//{ \
//    background: #444; \
//    border: 1px solid #000; \
//    background-color: QLinearGradient( \
//        x1:0, y1:0, \
//        x2:0, y2:1, \
//        stop:1 #212121, \
//        stop:0.4 #343434 \
//    ); \
//    margin-bottom:-1px; \
//    padding-bottom:1px; \
//} \
// \
//QMenu::item \
//{ \
//    padding: 2px 20px 2px 20px; \
//} \
// \
//QMenu::item:selected \
//{ \
//    color: #000000; \
//} \
// \
//QWidget:disabled \
//{ \
//    color: #404040; \
//    background-color: #323232; \
//} \
// \
//QAbstractItemView \
//{ \
//    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #4d4d4d, stop: 0.1 #646464, stop: 1 #5d5d5d); \
//} \
// \
//QLineEdit \
//{ \
//    background-color: #201F1F; \
//    padding: 1px; \
//    border-style: solid; \
//    border: 1px solid #1e1e1e; \
//    border-radius: 5; \
//} \
//QTextEdit \
//{ \
//    background-color: #201F1F; \
//    padding: 1px; \
//    border-style: solid; \
//    border: 1px solid #1e1e1e; \
//} \
// \
//QPushButton \
//{ \
//    color: #b1b1b1; \
//    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #565656, stop: 0.1 #525252, stop: 0.5 #4e4e4e, stop: 0.9 #4a4a4a, stop: 1 #464646); \
//    border-width: 1px; \
//    border-color: #1e1e1e; \
//    border-style: solid; \
//    border-radius: 6; \
//    padding: 3px; \
//    font-size: 12px; \
//    padding-left: 5px; \
//    padding-right: 5px; \
//} \
// \
//QPushButton:pressed \
//{ \
//    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2d2d2d, stop: 0.1 #2b2b2b, stop: 0.5 #292929, stop: 0.9 #282828, stop: 1 #252525); \
//} \
// \
//QComboBox \
//{ \
//    selection-background-color: #ffaa00; \
//    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #565656, stop: 0.1 #525252, stop: 0.5 #4e4e4e, stop: 0.9 #4a4a4a, stop: 1 #464646); \
//    border-style: solid; \
//    border: 1px solid #1e1e1e; \
//    border-radius: 5; \
//	min-width: 75px; \
//	color: silver; \
//} \
// \
//QComboBox:hover, QPushButton:hover, QAbstractSpinBox:hover, QLineEdit:hover, QSlider:hover \
//{ \
//    border: 1px solid QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
//} \
// \
//QComboBox:on \
//{ \
//    padding-top: 3px; \
//    padding-left: 4px; \
//    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2d2d2d, stop: 0.1 #2b2b2b, stop: 0.5 #292929, stop: 0.9 #282828, stop: 1 #252525); \
//    selection-background-color: #ffaa00; \
//} \
// \
//QComboBox QAbstractItemView \
//{ \
//    border: 2px solid darkgray; \
//    selection-background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
//} \
// \
//QComboBox::drop-down \
//{ \
//     subcontrol-origin: padding; \
//     subcontrol-position: top right; \
//     width: 15px; \
//     border-left-width: 0px; \
//     border-left-color: darkgray; \
//     border-left-style: solid; /* just a single line */ \
//     border-top-right-radius: 3px; /* same radius as the QComboBox */ \
//     border-bottom-right-radius: 3px; \
// } \
// \
//QComboBox::down-arrow \
//{ \
//	 width: 10; \
//	 height: 10; \
//     image: url(:/files/comboBox_arrow_icon.png); \
//} \
// \
//QScrollBar:horizontal { \
//     border: 1px solid #222222; \
//     background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0.0 #121212, stop: 0.2 #282828, stop: 1 #484848); \
//     height: 7px; \
//     margin: 0px 16px 0 16px; \
//} \
// \
//QScrollBar::handle:horizontal \
//{ \
//      background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #ffa02f, stop: 0.5 #d7801a, stop: 1 #ffa02f); \
//      min-height: 20px; \
//      border-radius: 2px; \
//} \
// \
//QScrollBar::add-line:horizontal { \
//      border: 1px solid #1b1b19; \
//      border-radius: 2px; \
//      background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #ffa02f, stop: 1 #d7801a); \
//      width: 14px; \
//      subcontrol-position: right; \
//      subcontrol-origin: margin; \
//} \
//	 \
//QScrollBar::sub-line:horizontal { \
//      border: 1px solid #1b1b19; \
//      border-radius: 2px; \
//      background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #ffa02f, stop: 1 #d7801a); \
//      width: 14px; \
//     subcontrol-position: left; \
//     subcontrol-origin: margin; \
//} \
// \
//QScrollBar::right-arrow:horizontal, QScrollBar::left-arrow:horizontal \
//{ \
//      border: 1px solid black; \
//      width: 1px; \
//      height: 1px; \
//      background: white; \
//} \
// \
//QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal \
//{ \
//      background: none; \
//} \
// \
//QScrollBar:vertical \
//{ \
//      background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0.0 #121212, stop: 0.2 #282828, stop: 1 #484848); \
//      width: 7px; \
//      margin: 16px 0 16px 0; \
//      border: 1px solid #222222; \
//} \
//  \
//QScrollBar::handle:vertical \
//{ \
//      background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 0.5 #d7801a, stop: 1 #ffa02f); \
//      min-height: 20px; \
//      border-radius: 2px; \
//} \
// \
//QScrollBar::add-line:vertical \
//{ \
//      border: 1px solid #1b1b19; \
//      border-radius: 2px; \
//      background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
//      height: 14px; \
//      subcontrol-position: bottom; \
//      subcontrol-origin: margin; \
//} \
//	 \
//QScrollBar::sub-line:vertical \
//{ \
//      border: 1px solid #1b1b19; \
//      border-radius: 2px; \
//      background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d7801a, stop: 1 #ffa02f); \
//      height: 14px; \
//      subcontrol-position: top; \
//      subcontrol-origin: margin; \
//} \
// \
//QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical \
//{ \
//      border: 1px solid black; \
//      width: 1px; \
//      height: 1px; \
//      background: white; \
//} \
// \
//QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical \
//{ \
//      background: none; \
//} \
// \
//QTextEdit \
//{ \
//    background-color: #242424; \
//	border: 1px solid #444; \
//} \
// \
//QPlainTextEdit \
//{ \
//    background-color: #242424; \
//	border: 1px solid #444; \
//} \
// \
//QHeaderView::section \
//{ \
//    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #616161, stop: 0.5 #505050, stop: 0.6 #434343, stop:1 #656565); \
//    color: white; \
//    padding-left: 4px; \
//    border: 1px solid #6c6c6c; \
//} \
// \
//QCheckBox:disabled \
//{ \
//	color: #414141; \
//} \
// \
//QDockWidget::title \
//{ \
//    text-align: center; \
//    spacing: 3px; /* spacing between items in the tool bar */ \
//    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #323232, stop: 0.5 #242424, stop:1 #323232); \
//} \
// \
//QDockWidget::close-button, QDockWidget::float-button \
//{ \
//    text-align: center; \
//    spacing: 1px; /* spacing between items in the tool bar */ \
//    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #323232, stop: 0.5 #242424, stop:1 #323232); \
//} \
// \
//QDockWidget::close-button:hover, QDockWidget::float-button:hover \
//{ \
//    background: #242424; \
//} \
// \
//QDockWidget::close-button:pressed, QDockWidget::float-button:pressed \
//{ \
//    padding: 1px -1px -1px 1px; \
//} \
// \
//QProgressBar \
//{ \
//    border: 2px solid grey; \
//    border-radius: 5px; \
//    text-align: center; \
//} \
// \
//QProgressBar::chunk \
//{ \
//    background-color: #d7801a; \
//    width: 2.15px; \
//    margin: 0.5px; \
//} \
// \
//QTabBar::tab { \
//    color: #b1b1b1; \
//    border: 1px solid #444; \
//    border-bottom-style: none; \
//    background-color: #323232; \
//    padding-left: 10px; \
//    padding-right: 10px; \
//    padding-top: 3px; \
//    padding-bottom: 2px; \
//    margin-right: -1px; \
//} \
// \
//QTabWidget::pane { \
//    border: 1px solid #444; \
//    top: 1px; \
//} \
// \
//QTabBar::tab:last \
//{ \
//    margin-right: 0; /* the last selected tab has nothing to overlap with on the right */ \
//    border-top-right-radius: 3px; \
//} \
// \
//QTabBar::tab:first:!selected \
//{ \
//	margin-left: 0px; /* the last selected tab has nothing to overlap with on the right */ \
//    border-top-left-radius: 3px; \
//} \
// \
//QTabBar::tab:!selected \
//{ \
//    color: #b1b1b1; \
//    border-bottom-style: solid; \
//    margin-top: 3px; \
//    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:1 #212121, stop:.4 #343434); \
//} \
// \
//QTabBar::tab:selected \
//{ \
//    border-top-left-radius: 3px; \
//    border-top-right-radius: 3px; \
//    margin-bottom: 0px; \
//	background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:1 #212121, stop:0.4 #343434, stop:0.2 #343434, stop:0.1 #ffaa00); \
//} \
// \
//QTabBar::tab:!selected:hover \
//{ \
//    /*border-top: 2px solid #ffaa00; \
//    padding-bottom: 3px;*/ \
//    border-top-left-radius: 3px; \
//    border-top-right-radius: 3px; \
//    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:1 #212121, stop:0.4 #343434, stop:0.2 #343434, stop:0.1 #ffaa00); \
//} \
// \
//QGroupBox { \
//    border: 1px solid white; \
//    border-radius: 2px; \
//    margin-top: 20px; \
//} \
// \
//QGroupBox::title { \
//    subcontrol-origin: margin; \
//    subcontrol-position: top center; \
//    padding-left: 10px; \
//    padding-right: 10px; \
//    padding-top: 10px; \
//} \
// \
//QRadioButton::indicator:checked, QRadioButton::indicator:unchecked{ \
//    color: #b1b1b1; \
//    background-color: #323232; \
//    border: 1px solid #b1b1b1; \
//    border-radius: 6px; \
//} \
// \
//QRadioButton::indicator:checked \
//{ \
//    background-color: qradialgradient( \
//        cx: 0.5, cy: 0.5, \
//        fx: 0.5, fy: 0.5, \
//        radius: 1.0, \
//        stop: 0.25 #ffaa00, \
//        stop: 0.3 #323232 \
//    ); \
//} \
// \
//QCheckBox, QRadioBox \
//{ \
//    background-color: transparent; \
//} \
// \
//QCheckBox::indicator, QGroupBox::indicator{ \
//    color: #b1b1b1; \
//    background-color: #201F1F; \
//    border: 1px solid #b1b1b1; \
//    width: 9px; \
//    height: 9px; \
//} \
// \
//QRadioButton::indicator \
//{ \
//    border-radius: 4px; \
//} \
// \
//QRadioButton::indicator:hover, QCheckBox::indicator:hover, QGroupBox::indicator:hover \
//{ \
//    border: 1px solid #ffaa00; \
//} \
// \
//QCheckBox::indicator:checked, QGroupBox::indicator:checked, QGroupBox::indicator:checked \
//{ \
//	background-color: qradialgradient( \
//        cx: 0.5, cy: 0.5, \
//        fx: 0.5, fy: 0.5, \
//        radius: 1.0, \
//        stop: 0.25 #ffaa00, \
//        stop: 0.3 #323232 \
//    ); \
//} \
// \
//QCheckBox::indicator:disabled, QRadioButton::indicator:disabled, QGroupBox::indicator:disabled \
//{ \
//	background-color: #323232; \
//    border: 1px solid #444; \
//} \
// \
//QSlider::groove:horizontal { \
//	border: 1px solid #3A3939; \
//	height: 8px; \
//	background: #201F1F; \
//	margin: 2px 0; \
//	border-radius: 2px; \
//} \
//	\
//QSlider::handle:horizontal { \
//	/*background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0.0 silver, stop: 0.2 #a8a8a8, stop: 1 #727272);*/ \
//	background-color: #ffaa00; \
//	border: 1px solid #3A3939; \
//	width: 14px; \
//	height: 14px; \
//	margin: -4px 0; \
//	border-radius: 2px; \
//} \
//	\
//QSlider::groove:vertical { \
//	border: 1px solid #3A3939; \
//	width: 8px; \
//	background: #201F1F; \
//	margin: 0 0px; \
//	border-radius: 2px; \
//} \
//	\
//QSlider::handle:vertical { \
//	/*background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0.0 silver, stop: 0.2 #a8a8a8, stop: 1 #727272);*/ \
//	background-color: #ffaa00; \
//	border: 1px solid #3A3939; \
//	width: 14px; \
//	height: 14px; \
//	margin: 0 -4px; \
//	border-radius: 2px; \
//} \
// \
//QGraphicsView { \
//    border: none; \
//} \
// \
//QAbstractSpinBox { \
//    padding-top: 2px; \
//    padding-bottom: 2px; \
//    border: 1px solid #3A3939; \
//    background-color: #201F1F; \
//    color: silver; \
//    border-radius: 2px; \
//    min-width: 75px; \
//} \
//QTableView QTableCornerButton::section { \
//	background: transparent; \
//} \
// \
//QComboBox \
//{ \
//    selection-background-color: #3d8ec9; \
//    background-color: #201F1F; \
//    border-style: solid; \
//    border: 1px solid #3A3939; \
//    border-radius: 2px; \
//    padding: 2px; \
//    min-width: 75px; \
//} \
// \
//QComboBox:on \
//{ \
//    background-color: #626873; \
//    padding-top: 3px; \
//    padding-left: 4px; \
//    selection-background-color: #4a4a4a; \
//} \
// \
//QComboBox QAbstractItemView \
//{ \
//    background-color: #201F1F; \
//    border-radius: 2px; \
//    border: 1px solid #444; \
//    selection-background-color: #3d8ec9; \
//} \
// \
//QComboBox::drop-down \
//{ \
//    subcontrol-origin: padding; \
//    subcontrol-position: top right; \
//    width: 15px; \
//    border-left-width: 0px; \
//    border-left-color: darkgray; \
//    border-left-style: solid; \
//    border-top-right-radius: 3px; \
//    border-bottom-right-radius: 3px; \
//} \
//")

#define CSS_CUSTOM_QWIDGET QString(" \
QWidget \
{ \
    color: #b1b1b1; \
    background-color: #323232; \
} \
QWidget:item:hover \
{ \
    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #ca0619); \
    color: #000000; \
} \
QWidget:item:selected \
{ \
    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
} \
QWidget:disabled \
{ \
    color: #404040; \
    background-color: #323232; \
}")

#define CSS_CUSTOM_QLABEL QString(" \
QLabel \
{ \
	background: transparent; \
}")

#define CSS_CUSTOM_QTOOLTIP QString(" \
QToolTip \
{ \
     border: 1px solid black; \
     background-color: #ffa02f; \
     padding: 1px; \
     border-radius: 3px; \
     opacity: 1000; \
}")

#define CSS_CUSTOM_QTOOLBAR QString(" \
QToolBar { \
	background: transparent; \
	border: 1px solid transparent; \
} \
QToolButton::checked { \
    border: 1px solid QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
}")

#define CSS_CUSTOM_QMENUBAR QString(" \
QMenuBar::item \
{ \
    background: transparent; \
} \
QMenuBar::item:selected \
{ \
    border: 1px solid #ffaa00; \
} \
QMenuBar::item:pressed \
{ \
    background: #444; \
    border: 1px solid #000; \
    background-color: QLinearGradient( \
        x1:0, y1:0, \
        x2:0, y2:1, \
        stop:1 #212121, \
        stop:0.4 #343434 \
    ); \
    margin-bottom:-1px; \
    padding-bottom:1px; \
}")

#define CSS_CUSTOM_QMENU QString(" \
QMenu::item \
{ \
    padding: 2px 20px 2px 20px; \
} \
QMenu::item:selected \
{ \
    color: #000000; \
}")

#define CSS_CUSTOM_QABSTRACTITEMVIEW QString(" \
QAbstractItemView \
{ \
    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #4d4d4d, stop: 0.1 #646464, stop: 1 #5d5d5d); \
}")

#define CSS_CUSTOM_QLINEEDIT QString(" \
QLineEdit \
{ \
    background-color: #201F1F; \
    padding: 1px; \
    border-style: solid; \
	border: 1px solid #3A3939; \
    border-radius: 5; \
} \
QLineEdit:hover \
{ \
    border: 1px solid QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
} \
QLineEdit:disabled \
{ \
    color: white; \
}")

#define CSS_CUSTOM_QTEXTEDIT QString(" \
QTextEdit \
{ \
    background-color: #201F1F; \
    padding: 1px; \
    border-style: solid; \
    border: 1px solid #1e1e1e; \
}")

#define CSS_CUSTOM_QPLAINTEXTEDIT QString(" \
QPlainTextEdit \
{ \
    background-color: #201F1F; \
    padding: 1px; \
    border-style: solid; \
    border: 1px solid #1e1e1e; \
}")

#define CSS_CUSTOM_QPUSHBUTTON QString(" \
QPushButton \
{ \
    color: #b1b1b1; \
    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #565656, stop: 0.1 #525252, stop: 0.5 #4e4e4e, stop: 0.9 #4a4a4a, stop: 1 #464646); \
    border-width: 1px; \
    border-color: #3A3939; \
    border-style: solid; \
    border-radius: 6; \
    padding: 3px; \
    font-size: 12px; \
    padding-left: 5px; \
    padding-right: 5px; \
} \
QPushButton:pressed \
{ \
    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2d2d2d, stop: 0.1 #2b2b2b, stop: 0.5 #292929, stop: 0.9 #282828, stop: 1 #252525); \
} \
QPushButton:hover \
{ \
    border: 1px solid QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
}")

#define CSS_CUSTOM_QSCROLLBAR QString(" \
QScrollBar:horizontal { \
     border: 1px solid #222222; \
     background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0.0 #121212, stop: 0.2 #282828, stop: 1 #484848); \
     height: 7px; \
     margin: 0px 16px 0 16px; \
} \
QScrollBar::handle:horizontal \
{ \
      background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #ffa02f, stop: 0.5 #d7801a, stop: 1 #ffa02f); \
      min-height: 20px; \
      border-radius: 2px; \
} \
QScrollBar::add-line:horizontal { \
      border: 1px solid #1b1b19; \
      border-radius: 2px; \
      background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #ffa02f, stop: 1 #d7801a); \
      width: 14px; \
      subcontrol-position: right; \
      subcontrol-origin: margin; \
} \
QScrollBar::sub-line:horizontal { \
      border: 1px solid #1b1b19; \
      border-radius: 2px; \
      background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #ffa02f, stop: 1 #d7801a); \
      width: 14px; \
     subcontrol-position: left; \
     subcontrol-origin: margin; \
} \
QScrollBar::right-arrow:horizontal, QScrollBar::left-arrow:horizontal \
{ \
      border: 1px solid black; \
      width: 1px; \
      height: 1px; \
      background: white; \
} \
QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal \
{ \
      background: none; \
} \
QScrollBar:vertical \
{ \
      background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0.0 #121212, stop: 0.2 #282828, stop: 1 #484848); \
      width: 7px; \
      margin: 16px 0 16px 0; \
      border: 1px solid #222222; \
} \
QScrollBar::handle:vertical \
{ \
      background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 0.5 #d7801a, stop: 1 #ffa02f); \
      min-height: 20px; \
      border-radius: 2px; \
} \
QScrollBar::add-line:vertical \
{ \
      border: 1px solid #1b1b19; \
      border-radius: 2px; \
      background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
      height: 14px; \
      subcontrol-position: bottom; \
      subcontrol-origin: margin; \
} \
QScrollBar::sub-line:vertical \
{ \
      border: 1px solid #1b1b19; \
      border-radius: 2px; \
      background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d7801a, stop: 1 #ffa02f); \
      height: 14px; \
      subcontrol-position: top; \
      subcontrol-origin: margin; \
} \
QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical \
{ \
      border: 1px solid black; \
      width: 1px; \
      height: 1px; \
      background: white; \
} \
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical \
{ \
      background: none; \
}")

#define CSS_CUSTOM_QHEADERVIEW QString(" \
QHeaderView::section \
{ \
    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #616161, stop: 0.5 #505050, stop: 0.6 #434343, stop:1 #656565); \
    color: white; \
    padding-left: 4px; \
    border: 1px solid #6c6c6c; \
}")

#define CSS_CUSTOM_QDOCKWIDGET QString(" \
QDockWidget::title \
{ \
    text-align: center; \
    spacing: 3px; /* spacing between items in the tool bar */ \
    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #323232, stop: 0.5 #242424, stop:1 #323232); \
} \
QDockWidget::close-button, QDockWidget::float-button \
{ \
    text-align: center; \
    spacing: 1px; /* spacing between items in the tool bar */ \
    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #323232, stop: 0.5 #242424, stop:1 #323232); \
} \
QDockWidget::close-button:hover, QDockWidget::float-button:hover \
{ \
    background: #242424; \
} \
QDockWidget::close-button:pressed, QDockWidget::float-button:pressed \
{ \
    padding: 1px -1px -1px 1px; \
}")

#define CSS_CUSTOM_QPROGRESSBAR QString(" \
QProgressBar \
{ \
    border: 2px solid grey; \
    border-radius: 5px; \
    text-align: center; \
} \
QProgressBar::chunk \
{ \
    background-color: #d7801a; \
    width: 2.15px; \
    margin: 0.5px; \
}")

#define CSS_CUSTOM_QTABBAR QString(" \
QTabBar::tab { \
    color: #b1b1b1; \
    border: 1px solid #444; \
    border-bottom-style: none; \
    background-color: #323232; \
    padding-left: 10px; \
    padding-right: 10px; \
    padding-top: 3px; \
    padding-bottom: 2px; \
    margin-right: -1px; \
} \
QTabWidget::pane { \
    border: 1px solid #444; \
    top: 1px; \
} \
QTabBar::tab:last \
{ \
    margin-right: 0; /* the last selected tab has nothing to overlap with on the right */ \
    border-top-right-radius: 3px; \
} \
QTabBar::tab:first:!selected \
{ \
	margin-left: 0px; /* the last selected tab has nothing to overlap with on the right */ \
    border-top-left-radius: 3px; \
} \
QTabBar::tab:!selected \
{ \
    color: #b1b1b1; \
    border-bottom-style: solid; \
    margin-top: 3px; \
    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:1 #212121, stop:.4 #343434); \
} \
QTabBar::tab:selected \
{ \
    border-top-left-radius: 3px; \
    border-top-right-radius: 3px; \
    margin-bottom: 0px; \
	background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:1 #212121, stop:0.4 #343434, stop:0.2 #343434, stop:0.1 #ffaa00); \
} \
QTabBar::tab:!selected:hover \
{ \
    /*border-top: 2px solid #ffaa00; \
    padding-bottom: 3px;*/ \
    border-top-left-radius: 3px; \
    border-top-right-radius: 3px; \
    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:1 #212121, stop:0.4 #343434, stop:0.2 #343434, stop:0.1 #ffaa00); \
}")

#define CSS_CUSTOM_QGROUPBOX QString(" \
QGroupBox { \
    border: 1px solid white; \
    border-radius: 2px; \
    margin-top: 20px; \
} \
QGroupBox::title { \
    subcontrol-origin: margin; \
    subcontrol-position: top center; \
    padding-left: 10px; \
    padding-right: 10px; \
    padding-top: 10px; \
} \
QGroupBox::indicator{ \
    color: #b1b1b1; \
    background-color: #201F1F; \
    border: 1px solid #b1b1b1; \
    width: 9px; \
    height: 9px; \
} \
QGroupBox::indicator:hover \
{ \
    border: 1px solid #ffaa00; \
}\
QGroupBox::indicator:checked \
{ \
	background-color: qradialgradient( \
        cx: 0.5, cy: 0.5, \
        fx: 0.5, fy: 0.5, \
        radius: 1.0, \
        stop: 0.25 #ffaa00, \
        stop: 0.3 #323232 \
    ); \
} \
QGroupBox::indicator:disabled \
{ \
	background-color: #323232; \
    border: 1px solid #444; \
}")

#define CSS_CUSTOM_QRADIOBUTTON QString(" \
QRadioButton \
{ \
    background-color: transparent; \
} \
QRadioButton:disabled \
{ \
	background-color: transparent; \
	color: #414141; \
} \
QRadioButton::indicator \
{ \
    border-radius: 4px; \
} \
QRadioButton::indicator:checked, QRadioButton::indicator:unchecked{ \
    color: #b1b1b1; \
    background-color: #201F1F; \
    border: 1px solid #b1b1b1; \
    border-radius: 6px; \
} \
QRadioButton::indicator:hover \
{ \
    border: 1px solid #ffaa00; \
} \
QRadioButton::indicator:disabled \
{ \
	background-color: #323232; \
    border: 1px solid #444; \
} \
QRadioButton::indicator:checked \
{ \
    background-color: qradialgradient( \
        cx: 0.5, cy: 0.5, \
        fx: 0.5, fy: 0.5, \
        radius: 1.0, \
        stop: 0.25 #ffaa00, \
        stop: 0.3 #323232 \
    ); \
	border-radius: 6px; \
}")

#define CSS_CUSTOM_QCHECKBOX QString(" \
QCheckBox \
{ \
    background-color: transparent; \
} \
QCheckBox:disabled \
{ \
	background-color: transparent; \
	color: #414141; \
} \
QCheckBox::indicator \
{ \
    color: #b1b1b1; \
    background-color: #201F1F; \
    border: 1px solid #b1b1b1; \
    width: 9px; \
    height: 9px; \
} \
QCheckBox::indicator:hover \
{ \
    border: 1px solid #ffaa00; \
} \
QCheckBox::indicator:disabled \
{ \
	background-color: #323232; \
    border: 1px solid #444; \
} \
QCheckBox::indicator:checked \
{ \
	background-color: qradialgradient( \
        cx: 0.5, cy: 0.5, \
        fx: 0.5, fy: 0.5, \
        radius: 1.0, \
        stop: 0.25 #ffaa00, \
        stop: 0.3 #323232 \
    ); \
}")
 
#define CSS_CUSTOM_QSLIDER QString(" \
QSlider::groove:horizontal { \
	border: 1px solid #3A3939; \
	height: 8px; \
	background: #201F1F; \
	margin: 2px 0; \
	border-radius: 2px; \
} \
QSlider::handle:horizontal { \
	/*background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0.0 silver, stop: 0.2 #a8a8a8, stop: 1 #727272);*/ \
	background-color: #ffaa00; \
	border: 1px solid #3A3939; \
	width: 14px; \
	height: 14px; \
	margin: -4px 0; \
	border-radius: 2px; \
} \
QSlider::groove:vertical { \
	border: 1px solid #3A3939; \
	width: 8px; \
	background: #201F1F; \
	margin: 0 0px; \
	border-radius: 2px; \
} \
QSlider::handle:vertical { \
	/*background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0.0 silver, stop: 0.2 #a8a8a8, stop: 1 #727272);*/ \
	background-color: #ffaa00; \
	border: 1px solid #3A3939; \
	width: 14px; \
	height: 14px; \
	margin: 0 -4px; \
	border-radius: 2px; \
} \
QSlider:hover \
{ \
    border: 1px solid QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
}")

#define CSS_CUSTOM_QGRAPHICSVIEW QString(" \
QGraphicsView { \
    border: none; \
}")

#define CSS_CUSTOM_QSPINBOX QString(" \
QSpinBox { \
    padding-top: 2px; \
    padding-bottom: 2px; \
    border: 1px solid #3A3939; \
    background-color: #201F1F; \
    color: silver; \
    border-radius: 2px; \
    min-width: 75px; \
} \
QSpinBox:hover \
{ \
    border: 1px solid QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
} \
QSpinBox:disabled \
{ \
    color: white; \
    background-color: #323232; \
}")

#define CSS_CUSTOM_QDOUBLESPINBOX QString(" \
QDoubleSpinBox { \
    padding-top: 2px; \
    padding-bottom: 2px; \
    border: 1px solid #3A3939; \
    background-color: #201F1F; \
    color: silver; \
    border-radius: 2px; \
    min-width: 75px; \
} \
QDoubleSpinBox:hover \
{ \
    border: 1px solid QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
} \
QDoubleSpinBox:disabled \
{ \
    color: white; \
    background-color: #323232; \
}")

#define CSS_CUSTOM_QTABLEVIEW QString(" \
QTableView QTableCornerButton::section { \
	background: transparent; \
}")

#define CSS_CUSTOM_QCOMBOBOX QString(" \
QComboBox \
{ \
    selection-background-color: #3d8ec9; \
    background-color: #201F1F; \
    border-style: solid; \
    border: 1px solid #3A3939; \
    border-radius: 2px; \
    padding: 2px; \
    min-width: 75px; \
} \
QComboBox:on \
{ \
    background-color: #626873; \
    padding-top: 3px; \
    padding-left: 4px; \
    selection-background-color: #4a4a4a; \
} \
QComboBox QAbstractItemView \
{ \
    background-color: #201F1F; \
    border-radius: 2px; \
    border: 1px solid #444; \
    selection-background-color: #3d8ec9; \
} \
QComboBox::down-arrow \
{ \
	 width: 10; \
	 height: 10; \
     image: url(:/files/comboBox_arrow_icon.png); \
} \
QComboBox::drop-down \
{ \
    subcontrol-origin: padding; \
    subcontrol-position: top right; \
    width: 15px; \
    border-left-width: 0px; \
    border-left-color: darkgray; \
    border-left-style: solid; \
    border-top-right-radius: 3px; \
    border-bottom-right-radius: 3px; \
} \
QComboBox:hover \
{ \
    border: 1px solid QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); \
} \
QComboBox:disabled \
{ \
    color: white; \
    background-color: #323232; \
}")

#define CSS_CUSTOM_QSPLITTER QString(" \
QSplitter::handle \
{ \
	background-color: #201F1F; \
    border: 1px solid #3A3939; \
} \
QSplitter::handle:hover \
{ \
    background-color: #787876; \
    border: 1px solid #3A3939; \
} \
QSplitter::handle:horizontal \
{ \
    width: 1px; \
} \
QSplitter::handle:vertical \
{ \
    height: 1px; \
}")

#define CSS_CUSTOM_STYLESHEET CSS_CUSTOM_QWIDGET + CSS_CUSTOM_QLABEL + CSS_CUSTOM_QTOOLTIP + \
							  CSS_CUSTOM_QTOOLBAR + CSS_CUSTOM_QMENUBAR + CSS_CUSTOM_QMENU + \
							  CSS_CUSTOM_QABSTRACTITEMVIEW + CSS_CUSTOM_QLINEEDIT + CSS_CUSTOM_QTEXTEDIT + \
							  CSS_CUSTOM_QPLAINTEXTEDIT + CSS_CUSTOM_QPUSHBUTTON + CSS_CUSTOM_QSCROLLBAR + \
							  CSS_CUSTOM_QHEADERVIEW + CSS_CUSTOM_QDOCKWIDGET + CSS_CUSTOM_QPROGRESSBAR + \
							  CSS_CUSTOM_QTABBAR + CSS_CUSTOM_QGROUPBOX + CSS_CUSTOM_QRADIOBUTTON + \
							  CSS_CUSTOM_QCHECKBOX + CSS_CUSTOM_QSLIDER + CSS_CUSTOM_QGRAPHICSVIEW + \
							  CSS_CUSTOM_QSPINBOX + CSS_CUSTOM_QDOUBLESPINBOX + CSS_CUSTOM_QTABLEVIEW + \
							  CSS_CUSTOM_QCOMBOBOX + CSS_CUSTOM_QSPLITTER

//#define CSS_CUSTOM_STYLESHEET CSS_CUSTOM_QMAINWINDOW + CSS_CUSTOM_QMANUBAR + \
//								CSS_CUSTOM_QMENU + CSS_CUSTOM_QTABWIDGET + CSS_CUSTOM_QGROUPBOX + \
//								CSS_CUSTOM_QLABEL + CSS_CUSTOM_QSPINBOX + CSS_CUSTOM_QDOUBLESPINBOX + \
//								CSS_CUSTOM_QCOMBOBOX + CSS_CUSTOM_QCHECKBOX + \
//								CSS_CUSTOM_QTEXTEDIT + CSS_CUSTOM_QTREEVIEW + CSS_CUSTOM_QTABLEWIDGET + \
//								CSS_CUSTOM_QHEADERVIEW + CSS_CUSTOM_QSCROLLAREA + CSS_CUSTOM_QWTPLOT + \
//								CSS_CUSTOM_QTOOLBAR + CSS_CUSTOM_QINPUTDIALOG + CSS_CUSTOM_QMESSAGEBOX + \
//								CSS_CUSTOM_QSTATUSBAR + CSS_CUSTOM_QGRAPHICSVIEW + CSS_CUSTOM_QLINEEDIT + \
//								CSS_CUSTOM_QRADIOBUTTON + CSS_CUSTOM_QPUSHBUTTON2 + CSS_CUSTOM_QTOOLBUTTON + \
//								CSS_CUSTOM_QSLIDER + CSS_CUSTOM_QSCROLLBAR

//#define CSS_CUSTOM_STYLESHEET CSS_CUSTOM_DARKORANGE

//#define CSS_CUSTOM_QWIDGET QString(" \
//								QWidget#CCMTuning \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//							    QWidget#CCMConfig \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//								QWidget#CCMConfigAdv \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//							    QWidget#LSHTuning \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//							    QWidget#LSHConfig \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//								QWidget#LSHConfigAdv \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//							    QWidget#LSHGridView \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//							    QDialog \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//								QWidget#LCATuning \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//								QWidget#LCAConfig \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//								QWidget#logDockWidget \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								} \
//								QWidget#previewDockWidget \
//								{ \
//									background-color: " + CSS_MAIN_COLOUR + "; \
//								}")

#define CSS_CUSTOM_VT_STYLESHEET /*CSS_CUSTOM_QWIDGET + */CSS_CUSTOM_STYLESHEET

//#define CSS_QGROUPBOX_SELECTED QString("QGroupBox { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 " + CSS_DARKEST_COLOUR + "); }")
#define CSS_QGROUPBOX_SELECTED QString("QGroupBox { background-color: rgba(0, 255, 0, 0.3); }")
//#define CSS_QWIDGET_CHANGED QString("QWidget#" + sender()->objectName() + " { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 #FF0000); color: #FFFFFF; }")
#define CSS_QWIDGET_CHANGED QString("QWidget#" + sender()->objectName() + " { background-color: rgba(255, 0, 0, 1.0); color: #FFFFFF; }")

} // namespace VisionLive

#endif // CSS_H
