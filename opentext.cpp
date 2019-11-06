#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QPlainTextEdit>
#include <QVBoxLayout>
using namespace std;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	QWidget *window = new QWidget();
	window->setWindowTitle("Opentext");
	QVBoxLayout *windowLayout = new QVBoxLayout();

	QPlainTextEdit *editor = new QPlainTextEdit();
	windowLayout->addWidget(editor);
	
	window->setLayout(windowLayout);
	window->show();
	return app.exec();
}
