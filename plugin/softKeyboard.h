#ifndef SOFTKEYBOARD_H
#define SOFTKEYBOARD_H

#include <QPointer>
#include <QWidget>

#include <QtCore/qglobal.h>

#if defined(PLUGIN_LIBRARY)
#define PLUGIN_EXPORT Q_DECL_EXPORT
#else
#define PLUGIN_EXPORT Q_DECL_IMPORT
#endif

class QPushButton;
class QLabel;

class PLUGIN_EXPORT SoftKeyboard final : public QWidget {
    Q_OBJECT

public:
    explicit SoftKeyboard(QWidget* parent = 0);
    ~SoftKeyboard();

    void clearChineseCache();

private:
    enum InputMode { zh,
        en,
        symb };

    void chineseCharactersUpdatePrevious();
    void chineseCharactersUpdateNext();
    void chineseCharactersSelected();
    void btnBackspaceClicked();
    void btnEnterClicked();
    void btnUpperClicked();
    void btnSymbolsClicked();
    void btnLanguageClicked();
    void btnBlankspaceClicked();
    void btnEmojiClicked();
    void characterButtonClicked();
    void updateKeyboard();
    void updateButtonStateOfChineseCharacters();
    void searchChineseCharacters(const int& currentpage);
    void hideKeyboard();
    void copyDictData();
    void sendKey(const QString& keytext);

    void initDecoder();

private:
    static bool isInit;

    const short max_spelling_length = 32;
    const short max_decoded_length = 32;
    unsigned short* cand_buf = new unsigned short[max_decoded_length];

    QList<QPushButton*> character_btns_list, chinese_characters_list, change_chinese_characters_page_list;
    QWidget *widget_keyboard, *widget_pinyin;
    QLabel* m_label_pinyin;
    QPointer<QObject> m_focusitem;
    int m_symbol_page;

    InputMode current_mode, last_mode;
    bool upper_mode;

signals:
    void sendKeyToFocusItem(const QString& keytext);
};

#endif // SOFTKEYBOARD_H
