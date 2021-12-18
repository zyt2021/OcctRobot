#ifndef ROBOTCREATOR_H
#define ROBOTCREATOR_H

#include <QString>
#include <QThread>

class Robot;

class RobotCreator : public QThread
{
    Q_OBJECT

public:
    explicit RobotCreator(QObject* parent = nullptr);
    RobotCreator(const QString &jsonPath, QObject* parent = nullptr);

    void SetJsonPath(const QString& str) {
        myJsonName = str;
    }
    void SetInput(Robot* input) {
        myInputRobot = input;
    }

protected:
    void run() override;

private:
     QString myJsonName;
     Robot* myInputRobot;

    void LoadJson(const QString &fileName);

signals:
    void createFinish();
};

#endif // ROBOTCREATOR_H
