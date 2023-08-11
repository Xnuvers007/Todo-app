#include <QApplication>
#include <QMainWindow>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QCheckBox>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    QWidget *centralWidget = new QWidget(&mainWindow);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QLabel *timeLabel = new QLabel(centralWidget);
    timeLabel->setAlignment(Qt::AlignHCenter);
    layout->addWidget(timeLabel);

    QListWidget *todoList = new QListWidget(centralWidget);
    layout->addWidget(todoList);

    QLineEdit *taskInput = new QLineEdit(centralWidget);
    layout->addWidget(taskInput);

    QPushButton *addButton = new QPushButton("Add Task", centralWidget);
    layout->addWidget(addButton);

    QPushButton *removeButton = new QPushButton("Remove Selected Task", centralWidget);
    layout->addWidget(removeButton);

    QCheckBox *themeToggle = new QCheckBox("Dark Theme", centralWidget);
    layout->addWidget(themeToggle);

    // Apply a dark theme using stylesheets
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    QPalette lightPalette = app.palette(); // Store the default (light) palette

    QObject::connect(themeToggle, &QCheckBox::toggled, [&](bool checked) {
        if (checked) {
            app.setPalette(darkPalette);
        } else {
            app.setPalette(lightPalette);
        }
    });

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        QTime currentTime = QTime::currentTime();
        QString timeText = currentTime.toString("hh:mm:ss");
        timeLabel->setText(timeText);
    });
    timer.start(1000); // Update every second

    QObject::connect(addButton, &QPushButton::clicked, [&]() {
    QString task = taskInput->text();
    if (!task.isEmpty()) {
        todoList->addItem(task);
        taskInput->clear();

        // Save the task to a file
        QFile file("tasks.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << task << endl;
        }

        // Display an alert pop-up dialog
        QMessageBox::information(&mainWindow, "Task Added", "Task has been added successfully.");
    }
});


    QObject::connect(removeButton, &QPushButton::clicked, [&]() {
        qDeleteAll(todoList->selectedItems());
    });

    // Load existing tasks from file
    QFile file("tasks.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString task = in.readLine();
            todoList->addItem(task);
        }
    }


    mainWindow.setCentralWidget(centralWidget);
    mainWindow.setWindowTitle("To-Do List Manager");
    mainWindow.show();

    return app.exec();
}
