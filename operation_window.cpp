#include "operation_window.h"
#include "ui_operation_window.h"
#include <fstream>
#include <vector>
#include <string>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QMessageBox>
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"
#include "DrawTree.h"
#include "TreeDraw.h"

operation_window::operation_window(const QString &filePath, QWidget *parent)
    : QWidget(parent), filePath(filePath), ui(new Ui::operation_window)
{
    ui->setupUi(this);

    this->setWindowState(Qt::WindowFullScreen);
}

operation_window::~operation_window()
{
    delete ui;
}

void operation_window::on_pushButton_clicked()
{
    if (filePath.isEmpty())
    {
        QMessageBox::warning(this, "Error", "No file selected.");
        return;
    }

    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Error", "Failed to open input file.");
        return;
    }

    QFileInfo fileInfo(filePath);
    QString outputFilePath = fileInfo.path() + "/token_file.txt";

    std::ifstream inFile(filePath.toStdString());
    std::ofstream outFile(outputFilePath.toStdString());
    if (!outFile.is_open())
    {
        QMessageBox::warning(this, "Error", "Failed to create output file.");
        return;
    }

    int lineCount = 1;
    std::string line;
    std::vector<Token> outputTokens;

    while (std::getline(inFile, line))
    {
        std::vector<Token> tokens = tokenize(line, lineCount);
        outputTokens.insert(outputTokens.end(), tokens.begin(), tokens.end());
        lineCount++;
    }

    // Write tokens to the output file
    for (const Token &token : outputTokens)
    {
        outFile << token.value << "," << token.type << "\n";
    }

    outFile.close();
    inFile.close();

    QMessageBox::information(this, "Success", "Tokens successfully written to:\n" + outputFilePath);
}

void operation_window::on_pushButton_2_clicked()
{
    if (filePath.isEmpty())
    {
        QMessageBox::warning(this, "Error", "No file selected.");
        return;
    }

    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Error", "Failed to open input file.");
        return;
    }

    QFileInfo fileInfo(filePath);
    QString outputFilePath = fileInfo.path() + "/token_file.txt";

    try
    {
        std::ifstream inFile(filePath.toStdString());
        std::ofstream outFile(outputFilePath.toStdString());
        if (!outFile.is_open())
        {
            QMessageBox::warning(this, "Error", "Failed to create output file.");
            return;
        }

        int lineCount = 1;
        std::string line;
        std::vector<Token> outputTokens;

        while (std::getline(inFile, line))
        {
            std::vector<Token> tokens = tokenize(line, lineCount);
            outputTokens.insert(outputTokens.end(), tokens.begin(), tokens.end());
            lineCount++;
        }

        // Write tokens to the output file
        for (const Token &token : outputTokens)
        {
            outFile << token.value << "," << token.type << "\n";
        }

        Parser parser(outputTokens);
        auto syntaxTree = parser.parse();

        // Draw the syntax tree
        TreeDraw *treeDraw = new TreeDraw(this);
        treeDraw->drawSyntaxTree(syntaxTree);
        treeDraw->show();

        QMessageBox::information(this, "Success", "Syntax tree drawn successfully and tokens saved.");
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Error", e.what());
    }
    catch (...)
    {
        QMessageBox::critical(this, "Error", "An unknown error occurred.");
    }
}
