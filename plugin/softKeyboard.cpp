#include "softKeyboard.h"
#include "pinyinime.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

using namespace ime_pinyin;

bool SoftKeyboard::isInit = false;

#define chinesecharacters_number 7
const char* keyboard_characters = "qwertyuiopasdfghjklzxcvbnm?";
const QString keyboard_symbols[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
    "@", "#", "_", "\"", "“", "”", ",", "，", ".", "。",
    ";", "；", ":", "：", "'", "’", "、", "!", "！",
    "~", "～", "+", "-", "*", "/", "=", "÷", "×", "√",
    "`", "?", "^", "&&", "%", "|", "(", ")", "（", "）",
    "[", "]", "【", "】", "{", "}", "<", ">", "《",
    "》", "$", "€", "￡", "￠", "￥", "§", "—", "／", "＼",
    "·", "……", "——", "→", "←", "↑", "↓", "■", "□", "●",
    "○", "『", "』", "「", "」", "★", "☆", "◆", "◇" }; // 29*3

SoftKeyboard::SoftKeyboard(QWidget* parent)
    : QWidget(parent)
    , m_focusitem(nullptr)
{
    character_btns_list.clear();
    current_mode = InputMode::en;
    upper_mode = false;
    m_symbol_page = 0;

    this->setMinimumHeight(192);
    int keyboard_btn_width = 800 / 11.5;
    int keyboard_btn_height = 240 / 5.0;

    // 设置主窗体样式
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);

    // 加载QSS样式表
    QFile qss(":/res/stylesheet.qss");
    if (false == qss.open(QFile::ReadOnly))
        return;
    this->setStyleSheet(qss.readAll());
    qss.close();

    // 图标字体
    int fontId = QFontDatabase::addApplicationFont(":/res/FontAwesome.otf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont btnicofont(fontName);
    btnicofont.setPixelSize(10);

    // 单行布局
    QHBoxLayout* hb[6];
    for (int i = 0; i < 6; i++) {
        hb[i] = new QHBoxLayout();
        hb[i]->setMargin(0);
        hb[i]->setSpacing(2);
    }

    widget_pinyin = new QWidget(this);
    widget_pinyin->setFixedHeight(keyboard_btn_height);
    widget_pinyin->setAttribute(Qt::WA_TranslucentBackground);
    // 拼音缓存
    m_label_pinyin = new QLabel(this);
    m_label_pinyin->setFixedHeight(keyboard_btn_height * 0.4);
    hb[0]->addWidget(m_label_pinyin);
    hb[0]->addStretch(1);

    // 汉子缓存
    for (int i = 0; i < chinesecharacters_number; i++) {
        QPushButton* btn = new QPushButton(this);
        btn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        btn->setFixedHeight(keyboard_btn_height * 0.6);
        hb[1]->addWidget(btn);
        if (i != chinesecharacters_number - 1)
            hb[1]->addStretch(1);
        if (i == 0 || i == chinesecharacters_number - 1) {
            change_chinese_characters_page_list.append(btn);
            btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            btn->setFont(btnicofont);
            btn->setText(i == 0 ? QString(QChar(0xf0d9)) : QString(QChar(0xf0da)));
            btn->setObjectName("hanzichangepage");
            i == 0 ? connect(btn, &QPushButton::clicked, this, &SoftKeyboard::chineseCharactersUpdatePrevious) : connect(btn, &QPushButton::clicked, this, &SoftKeyboard::chineseCharactersUpdateNext);
        } else {
            chinese_characters_list.append(btn);
            btn->setObjectName("hanzicandidates");
            connect(btn, &QPushButton::clicked, this, &SoftKeyboard::chineseCharactersSelected);
        }
    }

    // QPushButton* btn_hidekeyboard = new QPushButton(QString(QChar(0xf11c)).append(QChar(0xf103)));
    // btn_hidekeyboard->setFixedHeight(keyboard_btn_height * 0.6);
    // btn_hidekeyboard->setFont(btnicofont);
    // btn_hidekeyboard->setObjectName("function_button");
    // hb[1]->addWidget(btn_hidekeyboard);
    // connect(btn_hidekeyboard, &QPushButton::clicked, this, &KeyboardForm::hideKeyboard);

    QVBoxLayout* vb_pinyin = new QVBoxLayout(widget_pinyin);
    vb_pinyin->addLayout(hb[0]);
    vb_pinyin->addLayout(hb[1]);
    vb_pinyin->setMargin(0);
    vb_pinyin->setSpacing(0);

    widget_keyboard = new QWidget(this);
    widget_keyboard->setFixedHeight(keyboard_btn_height * 4.0);
    widget_keyboard->setAttribute(Qt::WA_TranslucentBackground);
    // 键盘
    for (int i = 0; i < 27; i++) {
        QPushButton* btn = new QPushButton(QChar(keyboard_characters[i]), this);
        btn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        btn->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
        character_btns_list.append(btn);
        connect(btn, &QPushButton::clicked, this, &SoftKeyboard::characterButtonClicked);
    }
    // 第一排字母:0-9
    for (int i = 0; i < 10; i++) {
        hb[2]->addWidget(character_btns_list.at(i));
        hb[2]->setStretch(i, 0);
    }

    // 第二排字母:10-18
    for (int i = 10; i < 19; i++) {
        hb[3]->addWidget(character_btns_list.at(i));
        hb[3]->setStretch(i - 10, 0);
    }

    // 第三排字母:20-26
    QPushButton* btn_upper = new QPushButton(QChar(0xf062));
    btn_upper->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    btn_upper->setFixedSize(keyboard_btn_width * 1.2, keyboard_btn_height);
    btn_upper->setFont(btnicofont);
    btn_upper->setObjectName("function_button");
    hb[4]->addWidget(btn_upper);

    character_btns_list.append(btn_upper);

    connect(btn_upper, &QPushButton::clicked, this, &SoftKeyboard::btnUpperClicked);
    for (int i = 19; i < 26; i++) {
        hb[4]->addWidget(character_btns_list.at(i));
    }
    QPushButton* btn_backspace = new QPushButton(QChar(0xf060));
    btn_backspace->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    btn_backspace->setFont(btnicofont);
    btn_backspace->setFixedSize(keyboard_btn_width * 1.2, keyboard_btn_height);
    btn_backspace->setObjectName("function_button");
    hb[4]->addWidget(btn_backspace);
    connect(btn_backspace, &QPushButton::clicked, this, &SoftKeyboard::btnBackspaceClicked);

    // 第四排功能键
    QPushButton* btn_symbols = new QPushButton(".?123");
    btn_symbols->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    btn_symbols->setFixedSize(keyboard_btn_width * 1.2, keyboard_btn_height);
    btn_symbols->setObjectName("function_button");
    hb[5]->addWidget(btn_symbols);
    connect(btn_symbols, &QPushButton::clicked, this, &SoftKeyboard::btnSymbolsClicked);

    QPushButton* btn_language = new QPushButton(QChar(0xf0ac));
    btn_language->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    btn_language->setFixedSize(keyboard_btn_width, keyboard_btn_height);
    btn_language->setFont(btnicofont);
    btn_language->setObjectName("function_button");
    hb[5]->addWidget(btn_language);
    connect(btn_language, &QPushButton::clicked, this, &SoftKeyboard::btnLanguageClicked);

    QPushButton* btn_blankspace = new QPushButton("英文");
    btn_blankspace->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    btn_blankspace->setFixedHeight(keyboard_btn_height);
    hb[5]->addWidget(btn_blankspace);
    character_btns_list.append(btn_blankspace);
    connect(btn_blankspace, &QPushButton::clicked, this, &SoftKeyboard::btnBlankspaceClicked);

    QPushButton* ask = character_btns_list.at(26);
    ask->setFixedSize(keyboard_btn_width, keyboard_btn_height);
    ask->setFont(btnicofont);
    ask->setObjectName("function_button");
    hb[5]->addWidget(ask);

    QPushButton* btn_enter = new QPushButton("换行");
    btn_enter->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    btn_enter->setFixedSize(keyboard_btn_width * 1.2, keyboard_btn_height);
    btn_enter->setObjectName("function_button");
    hb[5]->addWidget(btn_enter);
    connect(btn_enter, &QPushButton::clicked, this, &SoftKeyboard::btnEnterClicked);

    QVBoxLayout* vb_keyboard = new QVBoxLayout(widget_keyboard);
    vb_keyboard->setMargin(0);
    vb_keyboard->setSpacing(2);
    for (int i = 2; i < 6; i++) {
        vb_keyboard->addLayout(hb[i]);
    }

    QVBoxLayout* vb_system = new QVBoxLayout(this);
    vb_system->setMargin(0);
    vb_system->setSpacing(0);
    vb_system->addStretch(1);
    vb_system->addWidget(widget_pinyin);
    vb_system->addWidget(widget_keyboard);
    widget_pinyin->hide();

    updateButtonStateOfChineseCharacters();

    copyDictData();
    initDecoder();

    connect(this, &SoftKeyboard::sendKeyToFocusItem, this, &SoftKeyboard::sendKey);
    connect(qobject_cast<QApplication*>(QCoreApplication::instance()), &QApplication::focusChanged, this, [this](QWidget*, QWidget* now) {
        m_focusitem = now;
    });
}

SoftKeyboard::~SoftKeyboard()
{
}

void SoftKeyboard::initDecoder()
{
    if (!isInit) {
        QString app_dir("./dict/%1");
        isInit = im_open_decoder(app_dir.arg("dict_pinyin.dat").toLocal8Bit().data(), app_dir.arg("dict_pinyin_user.dat").toLocal8Bit().data());

        im_set_max_lens(max_spelling_length, max_decoded_length);

        im_reset_search();
    }
}

void SoftKeyboard::updateButtonStateOfChineseCharacters()
{
    if (m_label_pinyin->text().isEmpty()) {
        change_chinese_characters_page_list.at(0)->setEnabled(false);
        change_chinese_characters_page_list.at(1)->setEnabled(false);
    }
}

void SoftKeyboard::chineseCharactersUpdatePrevious()
{
    searchChineseCharacters(-1);
}

void SoftKeyboard::chineseCharactersUpdateNext()
{
    searchChineseCharacters(1);
}

void SoftKeyboard::chineseCharactersSelected()
{
    emit sendKeyToFocusItem(((QPushButton*)sender())->text());
    clearChineseCache();
}

void SoftKeyboard::btnBackspaceClicked()
{
    if (current_mode != InputMode::zh || m_label_pinyin->text().isEmpty()) {
        emit sendKeyToFocusItem("\x7F");
    } else {
        m_label_pinyin->setText(m_label_pinyin->text().left(m_label_pinyin->text().length() - 1));
        if (m_label_pinyin->text().isEmpty()) {
            clearChineseCache();
        } else {
            searchChineseCharacters(0);
        }
    }
}

void SoftKeyboard::btnEnterClicked()
{
    if (current_mode != InputMode::zh || m_label_pinyin->text().isEmpty()) {
        emit sendKeyToFocusItem("\n");
    } else {
        emit sendKeyToFocusItem(m_label_pinyin->text());
        clearChineseCache();
    }
}

void SoftKeyboard::btnUpperClicked()
{
    if (current_mode == InputMode::en) {
        upper_mode = !upper_mode;
    } else if (current_mode == InputMode::zh) {
        if (!m_label_pinyin->text().isEmpty() && m_label_pinyin->text().rightRef(1) != "'") {
            m_label_pinyin->setText(m_label_pinyin->text().append("'"));
        }
    } else {
        if (m_symbol_page == 0) {
            m_symbol_page = 1;
            character_btns_list.at(character_btns_list.length() - 2)->setText("2/3");
        } else if (m_symbol_page == 1) {
            m_symbol_page = 2;
            character_btns_list.at(character_btns_list.length() - 2)->setText("3/3");
        } else {
            m_symbol_page = 0;
            character_btns_list.at(character_btns_list.length() - 2)->setText("1/3");
        }
    }
    updateKeyboard();
}

void SoftKeyboard::btnSymbolsClicked()
{
    if (current_mode != InputMode::symb) {
        widget_pinyin->setHidden(true);
        character_btns_list.at(character_btns_list.length() - 1)->setText("符号");
        ((QPushButton*)sender())->setText("abc");
        last_mode = current_mode;
        current_mode = InputMode::symb;
        character_btns_list.at(character_btns_list.length() - 2)->setText("1/3");
    } else {
        ((QPushButton*)sender())->setText(".?123");
        current_mode = last_mode;
        m_symbol_page = 0;
    }
    upper_mode = false;
    updateKeyboard();
}

void SoftKeyboard::btnLanguageClicked()
{
    upper_mode = false;
    if (current_mode == InputMode::zh) {
        current_mode = InputMode::en;
        this->resize(QSize(this->width(), this->height() - 48));
    } else if (current_mode == InputMode::en) {
        current_mode = InputMode::zh;
        this->resize(QSize(this->width(), this->height() + 48));
    }
    if (current_mode != InputMode::symb) {
        last_mode = current_mode;
        updateKeyboard();
    }
}

void SoftKeyboard::clearChineseCache()
{
    m_label_pinyin->setText("");
    for (int i = 0; i < chinese_characters_list.length(); i++) {
        chinese_characters_list.at(i)->setText("");
        chinese_characters_list.at(i)->setHidden(true);
    }
    updateButtonStateOfChineseCharacters();
}

void SoftKeyboard::hideKeyboard()
{
    clearChineseCache();
    this->hide();
}

void SoftKeyboard::copyDictData()
{
    QDir app_dir;
    if (app_dir.mkpath("dict")) {
        QDir src(":/res/dict");
        foreach (const auto& file, src.entryInfoList()) {
            QFile f(file.absoluteFilePath());
            QString d(QString("dict").append(QDir::separator()).append(file.fileName()));
            f.copy(d);
        }
    }
}

void SoftKeyboard::sendKey(const QString& keytext)
{
    if (m_focusitem.isNull())
        return;

    if (keytext == QString("\x7F")) // Backspace <--
    {
        QCoreApplication::sendEvent(m_focusitem, new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier));
        QCoreApplication::sendEvent(m_focusitem, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Backspace, Qt::NoModifier));
    } else if (keytext == QString("\n")) {
        QCoreApplication::sendEvent(m_focusitem, new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier));
    } else if (keytext == QString("&&")) {
        QCoreApplication::sendEvent(m_focusitem, new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier, "&"));
        QCoreApplication::sendEvent(m_focusitem, new QKeyEvent(QEvent::KeyRelease, 0, Qt::NoModifier, "&"));
    } else {
        QCoreApplication::sendEvent(m_focusitem, new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier, keytext));
        QCoreApplication::sendEvent(m_focusitem, new QKeyEvent(QEvent::KeyRelease, 0, Qt::NoModifier, keytext));
    }
}

void SoftKeyboard::updateKeyboard()
{
    if (current_mode == InputMode::symb) {
        character_btns_list.at(character_btns_list.length() - 2)->setCheckable(false);
        for (int i = 0; i < 27; i++) {
            character_btns_list.at(i)->setText(keyboard_symbols[i + m_symbol_page * 27]);
        }
    } else {
        if (true == upper_mode && current_mode == InputMode::en) {
            character_btns_list.at(character_btns_list.length() - 2)->setCheckable(true);
            character_btns_list.at(character_btns_list.length() - 2)->setChecked(true);
            for (int i = 0; i < 26; i++) {
                character_btns_list.at(i)->setText(QChar(keyboard_characters[i]).toUpper());
            }
        } else {
            for (int i = 0; i < 26; i++) {
                character_btns_list.at(i)->setText(QChar(keyboard_characters[i]));
            }
        }
        if (current_mode == InputMode::en) {
            widget_pinyin->setHidden(true);
            character_btns_list.at(character_btns_list.length() - 3)->setText("?");
            character_btns_list.at(character_btns_list.length() - 2)->setText(QChar(0xf062));
            character_btns_list.at(character_btns_list.length() - 1)->setText("英文");
        } else if (current_mode == InputMode::zh) {
            character_btns_list.at(character_btns_list.length() - 2)->setCheckable(false);
            widget_pinyin->setHidden(false);
            character_btns_list.at(character_btns_list.length() - 3)->setText("？");
            character_btns_list.at(character_btns_list.length() - 2)->setText("分词");
            character_btns_list.at(character_btns_list.length() - 1)->setText("拼音");
        }
    }

    if (current_mode != InputMode::zh) {
        clearChineseCache();
    }
}

void SoftKeyboard::btnBlankspaceClicked()
{
    if (current_mode != InputMode::zh || m_label_pinyin->text().isEmpty()) {
        emit sendKeyToFocusItem(" ");
    } else {
        emit sendKeyToFocusItem(chinese_characters_list.at(0)->text());
        clearChineseCache();
    }
}

void SoftKeyboard::btnEmojiClicked()
{
    emit sendKeyToFocusItem("::)");
}

void SoftKeyboard::characterButtonClicked()
{
    if (current_mode == InputMode::zh) {
        if (((QPushButton*)sender())->text() == "，" || ((QPushButton*)sender())->text() == "。" || ((QPushButton*)sender())->text() == "？") {
            emit sendKeyToFocusItem(((QPushButton*)sender())->text());
        } else {
            if (m_label_pinyin->text().length() < 15) {
                m_label_pinyin->setText(m_label_pinyin->text().append(((QPushButton*)sender())->text()));
                searchChineseCharacters(0);
                updateButtonStateOfChineseCharacters();
            }
        }
    } else {
        emit sendKeyToFocusItem(((QPushButton*)sender())->text());
    }
}

void SoftKeyboard::searchChineseCharacters(const int& currentpage)
{
    static size_t sum_pages = 0, cur_page = 0;
    static size_t cand_num = 0;

    const size_t characters_list_len = chinese_characters_list.length();

    if (0 == currentpage) {
        QByteArray bytearray(m_label_pinyin->text().toUtf8());
        cand_num = im_search(bytearray.data(), bytearray.size());
        sum_pages = cand_num / characters_list_len;
        cur_page = 0;
    } else {
        if (currentpage == -1) {
            if (cur_page > 0)
                cur_page--;
        } else
            cur_page++;
    }

    if (0 == cur_page) {
        change_chinese_characters_page_list.at(0)->setEnabled(false);
        change_chinese_characters_page_list.at(1)->setEnabled(true);
    } else {
        change_chinese_characters_page_list.at(0)->setEnabled(true);
        if (cur_page == sum_pages - 1) {
            change_chinese_characters_page_list.at(1)->setEnabled(false);
        }
    }

    uint32 start = cur_page * characters_list_len;
    for (uint32 i = start; i < start + characters_list_len; i++) {
        const char16* cand = im_get_candidate(i, cand_buf, max_decoded_length);
        size_t index = i - start;
        chinese_characters_list.at(index)->setHidden(false);
        if (cand) {
            chinese_characters_list.at(index)->setText(QString::fromUtf16(cand));
        } else {
            chinese_characters_list.at(index)->setText("");
        }
    }
}
