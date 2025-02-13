#ifndef CLINEEDIT_H
#define CLINEEDIT_H

#include <QAction>
#include <QLineEdit>
#include <QPaintEvent>
#include <QPainter>
#include <QRegExpValidator>
#include <QScreen>

class CLineEdit : public QLineEdit {
    Q_OBJECT

    Q_PROPERTY(bool cRegExp MEMBER m_regExp WRITE setRegExp FINAL)

public:
    explicit CLineEdit(QWidget* parent = nullptr)
        : QLineEdit(parent)
        , m_clearAction(new QAction)
    {
        action();
    }
    explicit CLineEdit(const QString& text, QWidget* parent = nullptr)
        : QLineEdit(text, parent)
        , m_clearAction(new QAction)
    {
        action();
    }
    virtual ~CLineEdit()
    {
        if (m_clearAction != nullptr) {
            delete m_clearAction;
        }
    }

protected:
    void resizeEvent(QResizeEvent* e) override
    {
        setClearButtonEnabled(false);

        this->updateSytle();

        if (isEnabled()) {
            applyRegExp();
        }

        QLineEdit::resizeEvent(e);
    }

private:
    inline void setRegExp(int t)
    {
        this->m_regExp = t;
    }

    void updateSytle()
    {
        if (this->styleSheet().isEmpty()) {
            this->setStyleSheet(QString("#%1{padding-left:5px;border:1px solid rgb(236, 236, 236);border-radius:3px;} ")
                                    .arg(this->objectName()));
        }
    }

    void action()
    {
        if (isEnabled()) {
            m_clearAction->setIcon(QIcon(":/icon/assets/del.png"));
            connect(m_clearAction, &QAction::triggered, this, [this] { clear(); });

            connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
                if (text.isEmpty()) {
                    removeAction(m_clearAction);
                    m_showAction = false;
                } else {
                    if (!m_showAction) {
                        addAction(m_clearAction, ActionPosition::TrailingPosition);
                        m_showAction = true;
                    }
                }
            });
        }
    }

    void applyRegExp()
    {
        if (!validator()) {
            switch (m_regExp) {
            case 0:
                setValidator(new QRegExpValidator(QRegExp(QStringLiteral("^(\\d|[1-9]\\d{0,4})(?:\\.\\d{1,4})?$|(^\\t?$)")), this));
                break;
            default:
                break;
            }
        }
    }

private:
    QAction* m_clearAction;
    bool m_showAction = false;
    int m_regExp = -1;
};

#endif // CLINEEDIT_H
