#include "loggerWidget.h"
#include <QPushButton>

const char *LoggerWidget::iconResources[] =
{
    ":/new/prefix1/bug_add.png",
    ":/new/prefix1/bug.png",
    ":/new/prefix1/info.png",
    ":/new/prefix1/warning.png",
    ":/new/prefix1/stop.png"
};

STATIC_ASSERT(CORE_COUNT_OF(LoggerWidget::iconResources) == Log::LEVEL_LAST , wrong_number_of_icon_resources);

LoggerWidget::LoggerWidget(QWidget *parent)
    : QWidget(parent)
    , logFont(NULL)
{
	ui.setupUi(this);

	ui.tableWidget->setColumnWidth(0, 70);
	ui.tableWidget->setColumnWidth(1, 120);
	ui.tableWidget->setColumnWidth(2, 200);
	ui.tableWidget->setColumnWidth(3, 40);
	ui.tableWidget->setColumnWidth(4, 150);
	ui.tableWidget->setColumnWidth(5, 800);

	mLevelIcons   = new QIcon *      [Log::LEVEL_LAST];
	mLevelFilters = new QPushButton *[Log::LEVEL_LAST];

    QGridLayout *gridLayout = new QGridLayout(this);
    ui.filterGroupBox->setLayout(gridLayout);
    gridLayout->setContentsMargins(3,3,3,3);

    for (int i = 0; i < Log::LEVEL_LAST; i++)
	{
	    mLevelIcons[i] = new QIcon(iconResources[i]);
		QPushButton *filterButton = new QPushButton(this);
        filterButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		filterButton->setText(QString(""));
		filterButton->setIcon(*mLevelIcons[i]);
		filterButton->setIconSize(QSize(16,16));
		filterButton->setCheckable(true);
		filterButton->setChecked(true);
		gridLayout->addWidget(filterButton, i / 4, i % 4);
		mLevelFilters[i] = filterButton;
	}

	qRegisterMetaType<Log::Message>("Log::Message");
	logFont = new QFont("Courier", 8);
}

LoggerWidget::~LoggerWidget()
{
	for (int i = 0; i < Log::LEVEL_LAST; i++)
	{
		delete_safe(mLevelIcons  [i]);
		delete_safe(mLevelFilters[i]);
	}
	deletearr_safe(mLevelIcons);
	deletearr_safe(mLevelFilters);
	delete_safe(logFont);
}

void LoggerWidget::drain(Log::Message &message)
{
    DOTRACE(("Log is in the LoggerWidget drain"));
	QMetaObject::invokeMethod(this, "doDrain", Qt::QueuedConnection, Q_ARG(Log::Message, message));
	//doDrain(message);
}


void LoggerWidget::doDrain(Log::Message message)
{
    static const char *sLevels[] = { "D Debug", "Debug", "Info", "Warning", "Error" };

     while (ui.tableWidget->rowCount() > ui.scrollbackSpinBox->value())
        ui.tableWidget->removeRow(0);

    if (!mLevelFilters[message.get()->mLevel]->isChecked())
        return;

    int lastRow = ui.tableWidget->rowCount();
    ui.tableWidget->insertRow(lastRow);
    ui.tableWidget->setItem(lastRow, 0, new QTableWidgetItem((*mLevelIcons  [ message.get()->mLevel ]),
                                                             QString(sLevels[ message.get()->mLevel ]),            0));
    ui.tableWidget->setItem(lastRow, 1, new QTableWidgetItem(QString(time2str(message.get()->rawtime)),            0));
    ui.tableWidget->setItem(lastRow, 2, new QTableWidgetItem(QString(         message.get()->mOriginFileName),     0));
    ui.tableWidget->setItem(lastRow, 3, new QTableWidgetItem(QString::number( message.get()->mOriginLineNumber),   0));
    ui.tableWidget->setItem(lastRow, 4, new QTableWidgetItem(QString(         message.get()->mOriginFunctionName), 0));

    QTableWidgetItem *item =            new QTableWidgetItem(QString(         message.get()->s.str().c_str()),     0);
    item->setFont(*logFont);
    ui.tableWidget->setItem(lastRow, 5, item);

    if (ui.autoScrollCheckBox->isChecked()) {
        ui.tableWidget->scrollToBottom();
    }
//     ui.tableWidget->resizeRowsToContents();
}
