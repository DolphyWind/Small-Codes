#include "MainWindow.hpp"

MainWindow::~MainWindow()
{

}

void MainWindow::mainLoop() 
{
    while(window.isOpen())
    {
        handleEvents();
        update();
        draw();
    }
}

MainWindow::MainWindow(sf::Vector2u size, sf::String title, sf::Uint32 style) 
{
    std::srand(std::time(NULL));
    this->windowSize = size;
    this->windowTitle = title;
    this->windowStyle = style;
    this->window.create(sf::VideoMode(this->windowSize.x, this->windowSize.y), this->windowTitle, style);
    this->window.setFramerateLimit(60);
    #ifdef _WIN32
    homedir = getenv("appdata");
    homedirStr = homedir;
    if(!homedirStr.empty())
    {
        homedirStr += '\\';
        saveFolderPath = homedirStr += "AltmisSaniyedeTarih\\";
        try
        {
            if(!std::filesystem::is_directory(saveFolderPath)) std::filesystem::create_directory(saveFolderPath);
        }
        catch(const std::exception& e) {}

        filePath = saveFolderPath + "game.data";
        std::ifstream ifs(filePath, std::ios::binary);
        bool opened = ifs.good();
        if(opened)
            ifs.read((char*)&highScore, sizeof(std::int64_t));
        ifs.close();
        if(!opened) saveHighscore();
    }
    #else
    if((homedir = getenv("HOME")) == NULL)
        homedir = getpwuid(getuid())->pw_dir;
    homedirStr = homedir;
    if(!homedirStr.empty())
    {
        homedirStr += '/';
        saveFolderPath = homedirStr + ".AltmisSaniyedeTarih/";
        try
        {
            if(!std::filesystem::is_directory(saveFolderPath)) std::filesystem::create_directory(saveFolderPath);
        }
        catch(const std::exception& e) {}
        filePath = saveFolderPath + "game.data";
        std::ifstream ifs(filePath, std::ios::binary);
        bool opened = ifs.good();
        if(opened)
            ifs.read((char*)&highScore, sizeof(std::int64_t));
        ifs.close();
        if(!opened) saveHighscore();
    }
    #endif

    createQuestions();
    loadAssets();
    createMainMenu();
    createIngame();
    createEndScreen();
}

void MainWindow::handleEvents() 
{
    while (window.pollEvent(e))
    {
        if(e.type == sf::Event::Closed) window.close();
    }
}

void MainWindow::update() 
{
    if(!sf::Mouse::isButtonPressed(sf::Mouse::Left)) lPress = true;
    switch (this->currentScene)
    {
    case Scene::MainMenu:
        updateMainMenu();
        break;
    case Scene::Ingame:
        updateIngame();
        break;
    case Scene::EndScreen:
        updateEndScreen();
        break;
    }
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) lPress = false;
}

void MainWindow::draw() 
{
    window.clear(backgroundColor);
    
    switch (this->currentScene)
    {
    case Scene::MainMenu:
        drawMainMenu();
        break;
    case Scene::Ingame:
        drawIngame();
        break;
    case Scene::EndScreen:
        drawEndScreen();
        break;
    }

    window.display();
}

void MainWindow::createAnswerButton(Button &btn, sf::String str) 
{
    btn = Button(&window, &font, sf::Vector2f(266, 32), 14, str);
    btn.setNormalTexture(&buttonTexture);

    btn.setEventMode(EventMode::COLOR_WITH_TEXTURE);
    btn.setTextAlignment(TextAlignment::LEFT);
    btn.setNormalColor(sf::Color::White);
    btn.setHoverColor(sf::Color(225, 225, 225));
    btn.setPressedColor(sf::Color(100, 100, 100));
    btn.setTextColor(sf::Color::White);
    btn.setTextOffset(15, -2);
}

void MainWindow::createMainMenuButtons(Button &btn, sf::String str) 
{
    btn = Button(&window, &font, sf::Vector2f(200, 60), 18, str);
    btn.setEventMode(EventMode::COLOR);
    btn.setTextAlignment(TextAlignment::CENTER);
    btn.setTextOffset(0, -2);
    btn.setOutlineThickness(2);
    btn.setOutlineColor(sf::Color::Black);
}

void MainWindow::createTimerShape() 
{
    this->timerShape.setPointCount(361);
    this->outerTimer.setRadius(TIMER_RADIUS);
    this->outerTimer.setOutlineThickness(3);
    sf::Vector2f outerTimerPos = sf::Vector2f(1, 1) * this->outerTimer.getOutlineThickness();
    outerTimerPos.x = this->windowSize.x - outerTimerPos.x - 2 * TIMER_RADIUS;
    this->outerTimer.setOutlineColor(sf::Color::Black);
    this->outerTimer.setPosition(outerTimerPos);
    this->outerTimer.setFillColor(sf::Color::Transparent);
    this->timerShape.setFillColor(sf::Color::Red);
    this->timerShape.setPosition(outerTimerPos + sf::Vector2f(TIMER_RADIUS, TIMER_RADIUS));
}

void MainWindow::createTimerText() 
{
    this->timerText.setFont(font);
    this->timerText.setCharacterSize(18);
}

void MainWindow::calcPosTimerText() 
{
    sf::Vector2f outerTimerPos = this->outerTimer.getPosition();
    sf::Vector2f timerTextSize;
    timerTextSize.x = this->timerText.getLocalBounds().width + this->timerText.getLocalBounds().left;
    timerTextSize.y = this->timerText.getLocalBounds().height + this->timerText.getLocalBounds().top;

    this->timerText.setPosition(
        outerTimerPos.x + TIMER_RADIUS - (timerTextSize.x / 2),
        outerTimerPos.y + TIMER_RADIUS - (timerTextSize.y / 2) - 2
    );
}

void MainWindow::createUIButtons() 
{
    createAnswerButton(this->questionField, "Question goes here...");
    this->questionField.setSize(sf::Vector2f(626, 64));
    this->questionField.setNormalTexture(&this->questionFieldTexture);
    this->questionField.setDisabledColor(sf::Color::White);
    this->questionField.setDisabled(true);
    this->questionField.setPosition(0, 224);
    this->questionField.setTextAlignment(TextAlignment::TOPLEFT);
    this->questionField.setTextOffset(sf::Vector2f(80, 10));

    createAnswerButton(this->optionA, "A) ");
    createAnswerButton(this->optionB, "B) ");
    createAnswerButton(this->optionC, "C) ");
    createAnswerButton(this->optionD, "D) ");

    sf::Vector2f optAPos(24, this->questionField.getPosition().y + 96);
    this->optionA.setPosition(optAPos.x, optAPos.y);
    this->optionB.setPosition(optAPos.x + 313, optAPos.y);
    this->optionC.setPosition(optAPos.x, optAPos.y + 56);
    this->optionD.setPosition(optAPos.x + 313, optAPos.y + 56);
}

void MainWindow::loadAssets() 
{
    this->font.loadFromMemory(font_data, sizeof(font_data));
    this->buttonTexture.loadFromMemory(button_data, sizeof(button_data));
    this->questionFieldTexture.loadFromMemory(question_field_data, sizeof(question_field_data));
}

void MainWindow::updateTimerShape() 
{
    float elapsed = this->timerClock.getElapsedTime().asSeconds();
    if(elapsed >= maxTime)
    {
        this->currentScene = Scene::EndScreen;
        createEndText();
        isGrading = false;
        isBackwards = false;
        distanceGraded = 0;
        backgroundColor = originalBackgroundColor;
        return;
    }
    float measured = maxTime + 1 - elapsed;
    int measuredAsInt = measured;
    if(measuredAsInt != measuredAsIntPrev)
        this->timerAnim = !this->timerAnim;
    this->measuredAsIntPrev = measuredAsInt;
    float percentage = measured - measuredAsInt;
    int limitIndex = 360 * percentage;

    sf::Vector2f outerTimerPos = sf::Vector2f(1, 1) * this->outerTimer.getOutlineThickness();
    for(int i = 0; i < limitIndex; i++)
    {
        float deg = float(i) * M_PI / 180.f;
        if(!this->timerAnim)
            this->timerShape.setPoint(i, (float)TIMER_RADIUS * sf::Vector2f(-std::sin(deg), -std::cos(deg)));
        else
            this->timerShape.setPoint(i, sf::Vector2f(0, 0));
    }
    for(int i = limitIndex; i < 361; i++)
    {
        float deg = float(i) * M_PI / 180.f;
        if(!this->timerAnim)
            this->timerShape.setPoint(i, sf::Vector2f(0, 0));
        else
            this->timerShape.setPoint(i, (float)TIMER_RADIUS * sf::Vector2f(-std::sin(deg), -std::cos(deg)));
    }

    this->timerText.setString(std::to_string(measuredAsInt));
    calcPosTimerText();
}

void MainWindow::createIngame() 
{
    createUIButtons();
    createTimerShape();
    createTimerText();
    calcPosTimerText();
    createScoreText();
    selectQuestion(generateRandomIndex());
}

void MainWindow::updateIngame() 
{
    this->questionField.update();
    this->optionA.update();
    this->optionB.update();
    this->optionC.update();
    this->optionD.update();
    calcPosTimerText();
    updateTimerShape();
    gradeFunction();

    /*if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && lrPress)
    {
        currentIndex ++;
        if(currentIndex >= this->questions.size()) currentIndex = 0;
        this->selectQuestion(currentIndex);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && lrPress)
    {
        currentIndex --;
        if(currentIndex < 0) currentIndex = this->questions.size() - 1;
        this->selectQuestion(currentIndex);
    }
    lrPress = !(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right));
    */
    if(this->optionA.isClicked() && lPress)
    {
        if(this->questions[currentIndex]->getRightAnswer() == Answer::A) pickedRight();
        else pickedWrong();
        currentIndex = generateRandomIndex();
        this->selectQuestion(currentIndex);
    }
    if(this->optionB.isClicked() && lPress)
    {
        if(this->questions[currentIndex]->getRightAnswer() == Answer::B) pickedRight();
        else pickedWrong();
        currentIndex = generateRandomIndex();
        this->selectQuestion(currentIndex);
    }
    if(this->optionC.isClicked() && lPress)
    {
        if(this->questions[currentIndex]->getRightAnswer() == Answer::C) pickedRight();
        else pickedWrong();
        currentIndex = generateRandomIndex();
        this->selectQuestion(currentIndex);
    }
    if(this->optionD.isClicked() && lPress)
    {
        if(this->questions[currentIndex]->getRightAnswer() == Answer::D) pickedRight();
        else pickedWrong();
        currentIndex = generateRandomIndex();
        this->selectQuestion(currentIndex);
    }
}

void MainWindow::drawIngame() 
{
    this->questionField.draw();
    this->optionA.draw();
    this->optionB.draw();
    this->optionC.draw();
    this->optionD.draw();
    window.draw(this->outerTimer);
    window.draw(this->timerShape);
    window.draw(this->timerText);
    window.draw(scoreText);
}

void MainWindow::createMainMenu() 
{
    createMainMenuButtons(playButton, "Oyna");
    createMainMenuButtons(exitButton, sf::String(L"????k????"));
    sf::Vector2f playPos;
    playPos = (sf::Vector2f(this->windowSize.x, this->windowSize.y) - this->playButton.getSize()) / 2.f;
    playPos.y += 3 * this->playButton.getSize().y;
    this->playButton.setPosition(playPos);
    this->exitButton.setPosition(playPos.x, playPos.y + this->playButton.getSize().y + 10);
    this->titleText.setFont(font);
    this->titleText.setString("60 Saniyede Tarih");
    this->titleText.setCharacterSize(45);
    sf::Vector2f titlePos;
    sf::Vector2f titleTextSize;
    titleTextSize.x = this->titleText.getLocalBounds().width + this->titleText.getLocalBounds().left;
    titleTextSize.y = this->titleText.getLocalBounds().height + this->titleText.getLocalBounds().top;
    titlePos = (sf::Vector2f(this->windowSize.x, this->windowSize.y) - titleTextSize) / 2.f;
    titlePos.y = 100;
    this->titleText.setPosition(titlePos);
}

void MainWindow::updateMainMenu() 
{
    this->playButton.update();
    this->exitButton.update();
    if(this->playButton.isClicked() && lPress)
    {
        this->currentScene = Scene::Ingame;
        this->timerClock.restart();
        return;
    }
    if(this->exitButton.isClicked() && lPress)
    {
        this->window.close();
    }
}

void MainWindow::drawMainMenu() 
{
    this->playButton.draw();
    this->exitButton.draw();
    window.draw(this->titleText);
}

void MainWindow::createEndScreen() 
{
    createEndText();
    this->createMainMenuButtons(this->restartButton, L"Yeniden Oyna");
    this->createMainMenuButtons(this->mainMenuButton, L"Ana Men??");
    this->createMainMenuButtons(this->exitButtonE, L"????k????");
    float restartHeight = this->restartButton.getSize().y;
    sf::Vector2f restartPos = (sf::Vector2f(this->windowSize.x, this->windowSize.y) - this->restartButton.getSize()) / 2.f;
    restartPos.y += 2 * restartHeight;
    this->restartButton.setPosition(restartPos);
    this->mainMenuButton.setPosition(restartPos.x, restartPos.y + restartHeight + 10);
    this->exitButtonE.setPosition(restartPos.x, restartPos.y + 20 + 2 * restartHeight);
}

void MainWindow::updateEndScreen() 
{
    this->restartButton.update();
    this->mainMenuButton.update();
    this->exitButtonE.update();  

    if(this->restartButton.isClicked() && lPress)
    {
        this->currentScene = Scene::Ingame;
        createIngame();
        this->score = 0;
        this->timerClock.restart();
    }
    if(this->exitButtonE.isClicked() && lPress)
        this->window.close();
    if(this->mainMenuButton.isClicked() && lPress)
    {
        this->currentScene = Scene::MainMenu;
        createIngame();
        this->score = 0;
        this->timerClock.restart();
    }
}

void MainWindow::drawEndScreen() 
{
    for(int i = 0; i < 3; i++)
        window.draw(this->endScreenText[i]);
    this->restartButton.draw();
    this->mainMenuButton.draw();
    this->exitButtonE.draw();
}

void MainWindow::createEndText() 
{
    if(score > highScore)
    {
        highScore = score;
        saveHighscore();
    }
    for(int i = 0; i < 3; i++)
        this->endScreenText[i].setFont(this->font);
    this->endScreenText[0].setString(L"S??re Bitti!");
    this->endScreenText[1].setString(L"Skorun: " + std::to_wstring(score));
    this->endScreenText[2].setString(L"En Y??ksek Skor: " + std::to_wstring(highScore));
    for(int i = 0; i < 3; i++)
    {
        this->endScreenText[i].setPosition(
        (this->windowSize.x - this->endScreenText[i].getLocalBounds().width) / 2.f + this->endScreenText[i].getLocalBounds().left,
            96 + i * this->endScreenText[i].getLocalBounds().height + i * 10
        );
    }
}

void MainWindow::createQuestions() 
{
    this->questions.push_back(std::make_unique<Question>(
        L"Medine ??ehri \"Yesrib\" olarak an??l??rken sonras??nda bu ??ehre\n\"Medinet??'n-Nebi\" denilmesinin nedeni nedir?",
        L"I. Akabe Biat??",
        L"Hicret",
        L"Kerbela Olay??",
        L"Cemel Vakas??",
        Answer::B)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Buca?? ve Zoravna Antla??malar??yla Ukrayna hangi millete b??rak??lm????t??r?",
        L"Ruslar",
        L"Lehler",
        L"Kazaklar",
        L"Venedikliler",
        Answer::C)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"R??nesans hangi ??lkede ba??lam????t??r?",
        L"??talya",
        L"Almanya",
        L"??spanya",
        L"Fransa",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl??da ilk ba????ms??zl??????n?? kazanan millet hangisidir?",
        L"Yunanlar",
        L"K??r??ml??lar",
        L"Arnavutlar",
        L"S??rplar",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"??ark meselesi kim taraf??ndan ortaya at??lm????t??r?",
        L"Rusya",
        L"??ngiltere",
        L"Fransa",
        L"Osmanl??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Yaz??l?? metinlerde kullan??lan dilin lisan??n?? ????zen bilim dal?? hangisidir?",
        L"Filoloji",
        L"N??mizmatik",
        L"Epigrafya",
        L"Antropoloji",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"T??rkiye Sel??uklu Devleti'nde deniz ve donanma i??lerine bakan en ??st yetkili\nkimdir?",
        L"Emir'??s Sevahil",
        L"Emir'??l Umera",
        L"Emir-i Dad",
        L"Pervaneci",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl??da kale duvarlar?? ve surlar?? y??kmak i??in ??zel e??itim g??ren askeri\nbirim hangisidir?",
        L"La????mc??lar",
        L"Humbarac??lar",
        L"T??marl?? Sipahiler",
        L"M??sellemler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Kitab-?? Bahriye'nin yazar?? kimdir?",
        L"Piri Reis",
        L"Seyid Ali Reis",
        L"L??tf?? Pa??a",
        L"??bni Kemal",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"??stanbul'un y??netiminden, disiplininden ve g??venli??inden hangi devlet adam??\nsorumludur?",
        L"Veziriazam",
        L"Ni??anc??",
        L"Kazasker",
        L"Beylerbeyi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl?? Tarihinde 1718-1730 y??llar?? aras??nda ge??en d??nem hangisidir?",
        L"L??le Devri",
        L"Fetret Devri",
        L"Me??rutiyet Devri",
        L"Tanzimat Devri",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"\"Bir ??lke ne kadar madene ve paraya sahipse o kadar zengindir\" diyen anlay????\nhangisidir?",
        L"Merkantalizm",
        L"Emperyalizm",
        L"Makyavelizm",
        L"Rasyonalizm",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"\"??topya\" adl?? eserin yazar?? kimdir?",
        L"Thomas More",
        L"Machiavelli",
        L"J.J. Rousseau",
        L"Spinoza",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Bug??nk?? T??rkiye-??ran s??n??r??n?? b??y??k ??l????de belirleyen antla??ma hangisidir?",
        L"Kasr-?? ??irin",
        L"Nasuh Pa??a",
        L"Ferhat Pa??a",
        L"Serav",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Co??rafi Ke??ifler Osmanl?? Devleti'ni ??ncelikle hangi alanda etkilemi??tir?",
        L"Ekonomik",
        L"Siyasi",
        L"Askeri",
        L"Sosyal",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"L??le Devri'ni sona erdiren isyan a??a????dakilerden hangisidir?",
        L"Patrona Halil ??syan??",
        L"Ahmet Pa??a ??syan??",
        L"Kabak???? Mustafa ??syan??",
        L"Karayaz??c?? ??syan??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Basmac?? Haraketi'ne destek veren Osmanl?? devlet adam?? kimdir?",
        L"Enver Pa??a",
        L"Cemal Pa??a",
        L"Talat Pa??a",
        L"Yusuf Ak??ura",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"I. D??nya Sava????'nda imzalanan fakat y??r??rl??l????e girmeyen antla??ma\nhangisidir?",
        L"Sevr Antla??mas??",
        L"Versay Antla??mas??",
        L"Sen Germen Antla??mas??",
        L"N??yyi Antla??mas??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"II. D??nya Sava????'ndan ??nce izafiyet teorisi ile fizik biliminde ????????r a??an\nbilim insan?? kimdir?",
        L"Albert Einstein",
        L"Thomas Bohr",
        L"Thomas Morgan",
        L"Frank Whittle",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"T??rkiye'nin asker g??ndererek NATO'ya girmeyi ama??lad?????? sava?? hangisidir?",
        L"Kore Sava????",
        L"Vietnam Sava????",
        L"Arap-??srail Sava??lar??",
        L"K??rfez Sava????",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"ABD Ulusal Havac??l??k ve Uzay Dairesinin olarak faaliyet g??steren kurulu??\nedir? ",
        L"NASA",
        L"FBI",
        L"CIA",
        L"NSA",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"K??br??s Sorunu'nun ????z??m?? i??in ??al????malar yapm???? Birle??mi?? Milletler Genel\nSekreteri kimdir?",
        L"Kofi Annan",
        L"Bonki Moon",
        L"Christine Lagarde",
        L"Vladmir Putin",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Almanya'daki habsburg soyunun Alman prensleriyle yapt?????? sava?? nedir?",
        L"Otuz Y??l Sava??lar??",
        L"Kresy Sava????",
        L"??ifte G??l Sava????",
        L"Y??zy??l Sava??lar??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Reform haraketlerinin d??????nce ve fikir alan??ndaki sonucu nedir?",
        L"Skolastik d??????nce zay??flad??",
        L"Yeni mezhepler ortaya ????kt??",
        L"Mezhep kavgalar?? ba??lad??",
        L"Feodal y??netim zay??flad??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????daki isimlerden hangisi R??nesans'?? Fransa'da temsil etmemi??tir?",
        L"Erasmus",
        L"Montaigne",
        L"Pierre Lescot",
        L"Ronsard",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"R??nesans'??n sebepleri aras??nda a??a????dakilerden hangisi yer almaz?",
        L"Mezhep sava??lar??n??n ba??lamas??",
        L"Skolastik d??????ncenin zay??flamas??",
        L"Matbaan??n geli??tirilmesi",
        L"Antik d??nem eserlerinin incelenmesi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Yirmi Sekiz Mehmet ??elebi taraf??ndan yaz??lan, Osmanl??'n??n Bat??'ya a????lan\nilk penceresi olarak kabul edilen eser hangisidir?",
        L"Sefaretname",
        L"Risale",
        L"Mektubat",
        L"Salname",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl?? Devleti'nin Rusya ile m??cadelesinde tampon devlet olarak g??rd??????\ndevlet hangisidir?",
        L"Lehistan",
        L"??ran",
        L"Eflak",
        L"??sve??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"II. Mahmut d??neminde, merkezi otoriteyi yeniden sa??lamak i??in ayanlarla\nyap??lan s??zle??me hangisidir?",
        L"Sened-i ??ttifak",
        L"El-Ari??",
        L"Balta Liman??",
        L"Aynal??kavak",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????dakilerden hangisinin M??s??r sorunu ile bir ilgisi yoktur?",
        L"Edirne Antla??mas??",
        L"Londra Antla??mas??",
        L"Nizip Sava????",
        L"Vehhabilik Haraketi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Yunanistan'??n kurulu??u hangi antla??mayla sa??lanm????t??r?",
        L"Edirne Antla??mas??",
        L"Berlin Antla??mas??",
        L"K??tahya Antla??mas??",
        L"Zi??tovi Antla??mas??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????daki isyanlardan hangisi Ermeniler taraf??ndan ba??lat??lmam????t??r?",
        L"Mora ??syan??",
        L"Adana Olaylar??",
        L"Sason ??syan??",
        L"Van ??syan??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl?? Devleti K??r??m'??n Rusya'ya ait oldu??unu kabul etti??i antla??ma\nhangisidir?",
        L"Ya?? Antla??mas??",
        L"Zi??tovi Antla??mas??",
        L"B??kre?? Antla??mas??",
        L"Belgrad Antla??mas??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"\"??ark Meselesi\" kavram?? ilk kez nerede siyasi bir terim olarak\nkullan??lm????t??r?",
        L"Viyana Kongresi",
        L"Paris Konferans??",
        L"Berlin Konferans??",
        L"??stanbul Konferans??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Rusya ile Tilsit Antla??mas??'n?? imzalayan Avrupa devleti hangisidir?",
        L"Fransa",
        L"Avusturya",
        L"??ngiltere",
        L"Almanya",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl?? Devleti i??in ilk kez \"Hasta Adam\" ifadesini kullanan devlet\nhangisidir?",
        L"Rusya",
        L"Almanya",
        L"Fransa",
        L"Prusya",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Yunan isyan??n?? bast??rmas??ndan sonra Mora ve Girit Valiliklerini isteyen\nM??s??r Valisi kimdir?",
        L"Mehmet Ali Pa??a",
        L"Alemdar Mustafa Pa??a",
        L"Tepedelenli Ali Pa??a",
        L"Selim Pa??a",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????dakilerden hangisi n??fus at????lar??n??n kazan??lar??ndan biri de??ildir?",
        L"???? ve D???? g????ler",
        L"Sava?? g??c??n??n artmas??",
        L"Vergi gelirlerinin artmas??",
        L"??retim gelirlerinin artmas??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"XVII. y??zy??lda Avrupa'da kol g??c??n??n yerini makine g??c??n??n almas?? ile\nba??layan de??i??imin ad?? nedir?",
        L"Sanayi ??nk??lab??",
        L"Ak??l ??a????",
        L"Frans??z ??htilali",
        L"Helenistik ??a??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl??'n??n son d??nemlerinde sanayile??me ??abalar??n??n ba??ar??s??z\nolmas??n??n en ??nemli nedeni nedir?",
        L"Teknolojik geli??meleri takip etmemek",
        L"Yebi??eri isyanlar??",
        L"Ham maddenin azl??????",
        L"Madenlerin yetersizli??i",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????dakilerden hangisi Osmanl?? Devleti'nin sanayisi geli??tirmek i??in\nald?????? ??nlemlerdendir?",
        L"Ham madde sat??????n?? s??n??rlamak",
        L"Avrupa'dan ucuz mal almak",
        L"Ekonomik at??l??m ger??ekle??tirmek",
        L"Kapit??lasy??nlar?? s??n??rlamak",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????dakilerden hangisi kad??nlara y??nelik yay??n yapan gazete ve\ndergilerden de??ildir?",
        L"Terc??man-?? Hakikat",
        L"????kufezar",
        L"Terakki Gazetesi",
        L"Par??a Boh??as??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????dakilerden hangisi sanayile??menin olumsuz sonu??lar??ndan biri\nde??ildir?",
        L"K??lt??rel faaliyetlerin artmas??",
        L"Hava kirlili??i",
        L"Su s??k??nt??s??",
        L"??ehirlerin ??arp??k geli??mesi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????dakilerden hangisi t??ketim ekonomisinin ortaya ????kard?????? yeni i??\nalanlar??ndand??r?",
        L"Reklamc??l??k",
        L"Esnafl??k",
        L"Dokumac??l??k",
        L"Zanaatk??rl??k",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????daki ??ehirlerden hangisi T??rkiye Sel??uklular?? taraf??ndan\nfethedilmemi??tir?",
        L"Edirne",
        L"Konya",
        L"Sivas",
        L"Kayseri",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????dakilerden hangisi Malazgirt Sava????'ndan sonra Anadolu'da\nkurulan ilk d??nem beylikler aras??nda yer almaz?",
        L"Karamano??ullar??",
        L"Saltuklular",
        L"Artuklular",
        L"??aka Beyli??i",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????dakilerden hangisi B??y??k Sel??uklu topraklar??nda kurulan\natabeyliklerden de??ildir?",
        L"Saltuklular",
        L"Salgurlular",
        L"Zengino??ullar??",
        L"B??rililer",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi T??rk ??slam devletlerinde saray te??kilat??nda g??rev almaz?",
        L"Muhtesip",
        L"Hacip",
        L"Camedar",
        L"Emir-i Silah",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Divan??h??mayunun ??yeleri aras??nda a??a????dakilerden hangisi\ng??sterilemez?",
        L"Taht Kad??s??",
        L"Ni??anc??",
        L"Kazasker",
        L"M??ft??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl??'da dev??irme k??kenli devlet adamlar??n??n yeti??tirildi??i ve saray\nokulu olarak adland??r??lan b??l??m??n ad?? nedir?",
        L"Enderun",
        L"Birun",
        L"??tizam",
        L"Kapan",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"I. D??nya Sava???? sonras?? yeni kurulan devletler aras??nda a??a????dakilerden\nhangisi yer almaz?",
        L"Romanya",
        L"Macaristan",
        L"Yugoslavya",
        L"Polonya",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"K??seda?? Sava???? hangi iki devlet aras??nda yap??lm????t??r?",
        L"Mo??ollar - T??rkiye Sel??uklular??",
        L"Eyy??biler - Ha??l??lar",
        L"Meml??kler - Bizansl??lar",
        L"Harzem??ahlar - Mo??ollar",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????daki devletlerden hangisi Anadolu'da T??rk??eyi resmi devlet dili\nolarak kullanm????t??r?",
        L"Karamano??ullar??",
        L"Dani??mentliler",
        L"T??rkiye Sel??uklu Devleti",
        L"Karesio??ullar??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Ya????basan Medresesi hangi beyli??e aittir?",
        L"Dani??mentliler",
        L"Artuko??ullar??",
        L"Meng??cekler",
        L"Saltuklular",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"1096-1099 y??llar?? aras??nda yola ????kan Ha??l?? ordusu Kud??s'?? hangi devletin\nelinden alm????t??r?",
        L"F??t??m??ler",
        L"Musul Atabeyli??i",
        L"Eyyubiler",
        L"Meml??kler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Adaletin temsilcisi olan Osmanl?? idarecisi hangisidir?",
        L"Kad??",
        L"Suba????",
        L"??eyh??lislam",
        L"Muhtesip",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl??'da kamuya hizmet amac??yla yap??lan mimari eserler aras??nda hangisi\nyer almaz?",
        L"K??mbet",
        L"D??r??lkurr??",
        L"Kervansaray",
        L"Dar??????ifa",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Osmanl?? Erken ve Klasik D??nemi'nde geli??me g??steren sanat\ndallar??ndan biri de??ildir?",
        L"Heykel",
        L"Hat",
        L"Dokumac??l??k",
        L"??inicilik",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????daki camilerden hangisi osmanl??da yer almaz?",
        L"Selimiye Cami",
        L"S??leymaniye Cami",
        L"Beyazid Cami",
        L"??ehzade Cami",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi halk??n e??lenmesine imk??n sa??layan saray merasimlerinden biri\nde??ildir?",
        L"Cuma selaml??????",
        L"C??lus T??renleri",
        L"Hanedan ailesi d??????nleri",
        L"Askeri zaferler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi kurulu?? d??neminde, Anadolu'da T??rk siyasi birli??ini sa??lamak\namac??yla ele ge??irilen yerlerden biridir?",
        L"K??tahya",
        L"??zmit",
        L"??znik",
        L"Bursa",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi kurulu?? d??neminde Osmanl??'n??n Balkanlarda hakimiyet kurmak\ni??in yapt?????? sava??lardan biri de??ildir?",
        L"Koyunhisar",
        L"Varna",
        L"I. Kosova",
        L"II. Kosova",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangi sava??ta Bizans, Osmanl??'ya yenilerek Ha??l??lardan yard??m istemi??tir?",
        L"Maltepe Sava????",
        L"??imen Sava????",
        L"Ni??bolu Sava????",
        L"S??rps??nd?????? Sava????",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl??'n??n Balkanlar?? T??rk vatan?? yapmak i??in takip etti??i siyaset\nhangisidir?",
        L"??skan siyaseti",
        L"Fetih siyaseti",
        L"Bar???? siyaseti",
        L"Yay??lma siyaseti",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl??'n??n topraklar??na katt?????? ilk Anadolu T??rk beyli??i hangisidir?",
        L"Karesio??ullar??",
        L"Germiyano??ullar??",
        L"Hamito??ullar??",
        L"Candaro??ullar??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????daki sava??lar??n hangisinde Ha??l?? ??ttifak??'na kar???? Osmanl??'n??n\nyan??nda di??er anadolu beylikleri de yer alm????t??r?",
        L"I. Kosova",
        L"??irmen",
        L"Sazl??dere",
        L"Koyunhisar",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi XIV. y??zy??l ba??lar??nda Yak??n Do??u ve Avrupa'da bulunan\ndevletlerden de??ildir?",
        L"B??y??k Sel??uklu Devleti",
        L"??lhanl??lar",
        L"Meml??kl??ler",
        L"Alt??n Orda Devleti",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Osmanl??'n??n Balkanlardaki fetih faaliyetlerini h??zland??ran\nsava??lar aras??nda yer almaz?",
        L"Ankara Sava????",
        L"Sazl??dere Sava????",
        L"I. Kosova Sava????",
        L"I. ??imen Sava????",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangi beylik Osmanl?? taraf??ndan Anadolu T??rk Siyasi birli??ini sa??lamak\namac?? ile sonland??r??lmam????t??r?",
        L"Karakoyunlar",
        L"Karesio??ullar??",
        L"Ayd??no??ullar??",
        L"Germiyano??ullar??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl??daki ilk dini ve sosyal boyutlu isyan??n ad?? nedir?",
        L"??eyh Bedrettin ??syan??",
        L"Bu??uktepe ??syan??",
        L"D??zmece Mustafa Olay??",
        L"??ehzade Mustafa Olay??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Osmanl??'da tar??msal ??retimin devaml??l??????n?? sa??layan sistemdir?",
        L"T??mar sistemi",
        L"Vergi sistemi",
        L"Toprak sistemi",
        L"Millet sistemi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Osman Bey'in silah arkada??lar??ndan biri de??ildir?",
        L"Alp Arslan",
        L"Saltuk Alp",
        L"Konur Alp",
        L"Ak??a Koca",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Orhan Bey d??nemi kurulan ilk d??zenli ordu hangisidir?",
        L"Yaya ve m??sellemler",
        L"Yeni??eri Oca????",
        L"Acemi Oca????",
        L"T??marl?? Sipahiler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Yeni??eri Oca????n??n kurulmas??ndan sonra ??nc?? birlik olarak kullan??lan\nbirlik hangisidir?",
        L"Azebler",
        L"T??marl?? Sipahiler",
        L"Gaziy??n-?? R??m",
        L"Yaya ve m??sellemler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Alt?? B??l??k olarak da isimlendirilen Osmanl?? asker?? birli??i\nhangisidir?",
        L"Kap??kulu S??variler",
        L"T??marl?? Sipahiler",
        L"Cebeci Oca????",
        L"Yayalar ve m??sellemler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangi Osmanl?? askeri birli??i T??rk-??slam devletlerindeki gulam ordusuna\nbenzer?",
        L"Kap??kulu Askerleri",
        L"Sakac??lar",
        L"Ak??nc??lar",
        L"M??sellemler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Yeni??eri a??as??n??n ocakla ilgili i??leri g??rmek ??zere kurdu??u divan hangisidir?",
        L"A??a Div??n??",
        L"??kindi Div??n??",
        L"Ayak Div??n??",
        L"Galeve Div??n??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Dev??irme yoluyla al??nan ??ocuklara asker?? e??itim ilk hangi birimde\nverilir?",
        L"Acemi Oca????",
        L"Sipahiler",
        L"Humbarac??lar",
        L"Cebeci Oca????",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Acemi oca????nda e??itilen dev??irmelerin Yeni??eri Oca????na al??nmalar??na\nne ad verilir?",
        L"Kap??ya ????kma",
        L"Sanca??a ????kma",
        L"Kafes Us??l??",
        L"Pen??ik Sistemi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????daki tarikatlardan hangisi Anadolu'nun islamla??mas??nda etkin\nbir rol almam????t??r?",
        L"Vehhabilik",
        L"Rufailik",
        L"Yesevilik",
        L"Bekta??ilik",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????dakilerden hangisi Seyfiye s??n??f??n??n temel g??revleri aras??ndad??r?",
        L"Askerlik",
        L"B??rokrasi",
        L"Sa??l??k",
        L"E??itim",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Mevlana, Yunus Emre ve Hac?? Bekta?? Veli ile ba??layan edebiyat ak??m??\nhangisidir?",
        L"Tekke Edebiyat??",
        L"Divan Edebiyat??",
        L"A????k Edebiyat??",
        L"Halk Edebiyat??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi ilmiye s??n??fan mensup de??ildir?",
        L"Reis??lk??ttab",
        L"M??derris",
        L"??eyh??lislam",
        L"Kazasker",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"A??a????daki mutasavv??flardan hangisi Anadolu'da ya??amam????t??r?",
        L"Ahmet Yesevi",
        L"Ah?? Evran",
        L"Hac?? Bekt????-?? Vel??",
        L"Hac?? Bayram-?? Vel??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangiye Seyfiye s??n??f??na mensup de??ildir?",
        L"Defterdar",
        L"Vezirler",
        L"Beylerbeyi",
        L"Sancakbeyi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Fatih Sultan Mehmet'in Bizans'??n yeniden kurulmas??n?? ??nlemek i??in\ntopraklar??na katt?????? yer neresidir?",
        L"Mora Rum Despotluklar??",
        L"Arnavutluk Prensli??i",
        L"Eflay Voyvodal??????",
        L"??sfendiyaro??ullar?? Beyli??i",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl?? ??stanbul'u fethedip ticaret yollar?? ??zerindeki denetimini\nartt??r??nca Avrupal??lar ne yapm????t??r?",
        L"Co??rafi ke??ifler",
        L"Hristiyan birli??i",
        L"G??mr??k birli??i",
        L"Skolastik d??????nce",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"I. Elizabeth'in ittifak aray??????nda oldu??u \"B??y??k T??rk\" hangi Osmanl??\npadi??ah??d??r?",
        L"I. S??leyman",
        L"II. Mehmet",
        L"II. Bayezid",
        L"I. Selim",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl?? tarihinin yaz??lmas??n?? ilk emreden padi??ah kimdir?",
        L"II. Bayezit",
        L"II. Murat",
        L"II. Mehmet",
        L"I. Murat",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Birun'da yer almaz?",
        L"Beylerbeyi",
        L"Top??ular",
        L"Mehterler",
        L"??avu??lar",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Osmanl??da m??lkiyeti devlete ait olan topraklara ne ad verilir?",
        L"Mir?? arazi",
        L"????ri arazi",
        L"Vak??f arazi",
        L"M??lk arazi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi mir?? arazisinin b??l??mlerinden biridir?",
        L"Dirlik",
        L"Mukataa",
        L"Ocakl??k",
        L"Yurtluk",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"M??sl??man ??ift??ilerden al??nan ??r??n vergisine ne ad verilir?",
        L"??????r",
        L"Cizye",
        L"??iftbozan",
        L"Adetia??nam",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi vak??flar??n faaliyet alanlar?? aras??nda yer almaz?",
        L"Yeni??erilere ulufe vermek",
        L"K??pr?? yapmak",
        L"Cami yapmak",
        L"A??evleri kurmak",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Orta Asya g????lerinin sebepleri aras??nda yer almaz?",
        L"Ticareti geli??tirme iste??i",
        L"A????r?? n??fus art??????",
        L"Kurakl??k",
        L"Hayvan hastal??klar??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Asya Hun Devleti hakk??ndaki ilk bilgileri hangi kaynak vermektedir?",
        L"??in kaynaklar??",
        L"Orhun an??tlar??",
        L"Arkeolojik kaz??lar",
        L"Mezar ta??lar??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi kurultay ile ilgili yanl???? bir bilgidir?",
        L"Hatun ba??kanl??k yapabilir",
        L"??yelerine Toygun denir",
        L"Y??lda en az ???? kez toplan??r",
        L"Boy beyleri de kat??l??r",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Asya Hun Devleti ile ??in aras??ndaki m??cadelenin temel sebebi nedir?",
        L"??pek yolu h??kimiyeti",
        L"Siyasi birli??i sa??lama",
        L"K??rk yolu egemenli??i",
        L"Tar??msal alanlar?? ele ge??irme",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"\"T??rk\" ad??ndan ilk olarak hangi metinde bahsedilmi??tir?",
        L"??in y??ll??klar??nda",
        L"??ran belgelerinde",
        L"Yenisey Yaz??tlar??nda",
        L"K??k T??rk Kitabelerinde",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"??slamiyet ??ncesi kurulan T??rk devletlerinin ikiye b??l??nmesi neyin\nsonucudur?",
        L"??kili te??kilat",
        L"Kut anlay??????",
        L"Yar?? g????ebe ya??am",
        L"G??ktanr?? inanc??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Orta ??a?? Avrupa's??nda g??r??len sosyal s??n??flardand??r?",
        L"Burjuva",
        L"Brahmanlar",
        L"Paryalar",
        L"Sudralar",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Do??u Roma ??mparatorlu??u'nun resmi mezhebi nedir?",
        L"Ortadoksluk",
        L"Protestanl??k",
        L"Katoliklik",
        L"Kalvinizm",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Orta ??a?? Avrupa's??nda Ortadoks mezhebinin merkezi neresidir?",
        L"Konstantinapolis",
        L"Roma",
        L"Viyana",
        L"Belgrat",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"??slamiyet'ten ??nceki d??nemlerde Mekke, Medine ve Taif ??ehirlerinin\nbulundu??u b??lgeye ne ad verilirdi?",
        L"Hicaz",
        L"Yemen",
        L"Necid",
        L"Himyeri",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangi geli??meden sonra mekkeli m????rikler M??sl??manlar??n varl??????n??\nhukuken tan??m????t??r?",
        L"Hudeybiye Bar??????",
        L"Teb??k Seferi",
        L"Akabe Biat??",
        L"Medine S??zle??mesi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"M??sl??manlarla Bizans aras??nda yap??lan ilk sava????n ad?? nedir?",
        L"Mute Sava????",
        L"Huneyn Sava????",
        L"Ecnadin Sava????",
        L"Kadisiye Sava????",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Hz. Osman D??neminde ger??ekle??memi??tir?",
        L"S??ff??n Sava????'n??n yap??lmas??",
        L"Kur'an-?? Kerim'in ??o??alt??lmas??",
        L"Tunus'un ele ge??irilmesi",
        L"K??br??s'??n vergiye ba??lanmas??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Muaviye'nin ??am'da halifeli??ini ilan etmesi nedeniyle ????kan sava?? nedir?",
        L"S??ff??n sava????",
        L"Cemel Sava????",
        L"Yerm??k Sava????",
        L"Ecnadin Sava????",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"M??sl??manlar??n Avrupa'da ilerleyi??inin durmas?? a????s??ndan d??n??m noktas??\nolan sava????n ad?? nedir?",
        L"Puvatya Sava????",
        L"Kadiks Sava????",
        L"Celula Sava????",
        L"Kadisiye Sava????",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Emevi Devleti'nde ilk paran??n bast??r??lmas?? hangi h??k??mdar??n d??neminde\nger??ekle??mi??tir?",
        L"Abd??lmelik b. Merv??n",
        L"Muaviye",
        L"II.Velid",
        L"Yezid",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"End??l??s Emevi Devleti'nin kurucusu kimdir?",
        L"Abdurrahman",
        L"Muaviye",
        L"Hi??am",
        L"??mer Bin Abd??laziz",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"End??l??s Emevi Devleti'nin ba??kenti neresidir?",
        L"Kurtuba",
        L"??am",
        L"Ba??dat",
        L"Kufe",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Abbasi Devleti'nde merkezi otoritenin zay??flamas?? ile M??s??r'da ortaya\n????kan T??rk devletinin ad?? nedir?",
        L"Ih??idler",
        L"Sel??uklular",
        L"Karahanl??lar",
        L"Gazneliler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"El-Kanun Fi't-T??b adl?? eseri yazan T??rk bilim insan?? kimdir?",
        L"??bn-i Sina",
        L"Farabi",
        L"El-Biruni",
        L"Gazali",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"\"2. ????retmen\" lakab??yla an??lan ??slam alimi kimdir?",
        L"Farabi",
        L"??bn-i Sina",
        L"??mam-?? Azam",
        L"??mam Gazali",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Emevilerin y??k??lmas?? nedenleri aras??nda g??sterilemez?",
        L"T??rkler'in ??slamiyeti kabul??",
        L"Fetihlerin durmas??",
        L"??????lerin faaliyetleri",
        L"Arap milliyet??ili??i politikas??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Mutluluk veren bilgi anlam??na gelen eser hangisidir?",
        L"Kutadgu Bilig",
        L"Divan-?? L??gat'it T??rk",
        L"Atabet-??l Hakay??k",
        L"Divan-?? Hikmet",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"B??y??k Sel??uklu ile Bizans aras??nda yap??lan ilk sava?? hangisdir?",
        L"Pasinler Sava????",
        L"Miryokefalon Sava????",
        L"Malazgirt Sava????",
        L"Katvan Sava????",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi B??y??k Sel??uklu'nun y??k??lmas??yla ortaya ????kmam????t??r?",
        L"M??s??r Sel??uklular??",
        L"T??rkiye Sel??uklular??",
        L"Suriye Sel??uklular??",
        L"Kirman Sel??uklular??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Romen Diyojen'in Bizans'?? T??rk tehlikesinden korumak i??in yapt?????? sava??\nhangisidir?",
        L"Malazgirt Sava????",
        L"Miryokefalon Sava????",
        L"Hittin Sava????",
        L"Pasinler Sava????",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"??lk T??rk gramer kitabi ve ilk T??rk??e ansiklopedik s??zl??k olarak da kabul\nedilen eserin ad?? nedir?",
        L"Divan-?? L??gat'it T??rk",
        L"Kutadgu Bilig",
        L"??ehname",
        L"Atabet-??l Hakay??k",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Karahanl??lar??n Do??u ve Bat?? Karahanl??lar olarak ikiye ayr??lmas??n??n sebebi\nnedir?",
        L"Taht kavgalar??",
        L"O??uz isyanlar??",
        L"Gazneli sald??r??lar??",
        L"??in entrikalar??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Galileo'dan 600 sene ??nce d??nyan??n d??nd?????? fikrini savunan bilgin\nkimdir?",
        L"Biruni",
        L"Gazali",
        L"Harezm??",
        L"Farabi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"B??y??k Sel??uklularda ??ehzadelerin e??itimi veya ba????ms??z olarak bir\neyaletin y??netimi ile g??revli vezirlere ne ad verilir?",
        L"Atabey",
        L"Lala",
        L"Kadi-I Kudat",
        L"Hacip",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi B??y??k sel??uklular??n y??k??lma nedenleri aras??nda g??sterilemez?",
        L"Bizans'??n bask??s??",
        L"O??uzlar??n isyan??",
        L"Katvan Sava????",
        L"B??t??nilerin faaliyetleri",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi B??y??k Sel??uklulara ba??kentlik yapmam????t??r?",
        L"Semerkant",
        L"Merv",
        L"Cend",
        L"Rey",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Kast sistemi hangi medeniyette ortaya ????km????t??r?",
        L"Hint",
        L"S??mer",
        L"Babil",
        L"M??s??r",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"1347-1351 y??llar?? aras??nda Avrupa'n??n neredeyse yar??s??n??n ??l??m??ne\nsebep olan geli??me nedir?",
        L"Veba salg??n??",
        L"Do??al afet",
        L"???? isyanlar",
        L"Otuz Y??l Sava??lar??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Patrici-Plep aras??ndaki s??n??f m??cadelesi sonucu haz??rlanan Roma hukuk\nkurallar??na ne ad verilir?",
        L"On ??ki Levha Kanunlar??",
        L"Solon Kanunu",
        L"Dragon Kanunu",
        L"Psistaros Kanunu",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"D??nyan??n ilk kentsel yerle??imi olarak kabul edilen ??ehrin ad?? nedir?",
        L"??atalh??y??k",
        L"Yar??mburgaz",
        L"Lask??",
        L"Beldibi",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Uygarl??klar??n do??u??unda hangisi etkili de??ildir?",
        L"Tropikal ya??murlar",
        L"Siyasi durum",
        L"Ekonomi",
        L"Dinsel yap??",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Eski Yunan uygarl??klar??nda ??ehir devletlerine verilen isim nedir?",
        L"Polis",
        L"Site",
        L"Nom",
        L"Tekfur",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Tarihin ilk yaz??l?? antla??mas?? hangi iki devlet aras??nda ger??ekle??mi??tir?",
        L"Hitit-M??s??r",
        L"??ran-M??s??r",
        L"Asur-Hitit",
        L"Frigya-Lidya",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"M??s??r uygarl??????nda takvimin icat edilmesindeki en etkili olay nedir?",
        L"Nil Nehri'nin ta??mas??",
        L"G??ne?? tutulmas??",
        L"Ay tutulmas??",
        L"Mimarideki ilerlemeler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangi uygarl??k bug??nk?? modern alfabenin temelini atm????t??r?",
        L"Fenikeliler",
        L"Yunanl??lar",
        L"Urartular",
        L"Romal??lar",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Orta Asya k??lt??r merkezleri aras??nda hangisi yer almaz?",
        L"Truva",
        L"Tagar",
        L"Afanesyevo",
        L"Karasuk",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Bozk??r??n Kuyumcular?? olarak da bilinen T??rk toplulu??unun ad?? nedir?",
        L"Sakalar",
        L"Hunlar",
        L"K??kt??rkler",
        L"Uygurlar",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Anadolu'da ilk siyasal birli??i kuran uygarl??k hangisidir?",
        L"Hititler",
        L"Hattiler",
        L"S??merler",
        L"Frigler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi Anadolu'da kurulan ??lk ??a?? medeniyetlerinden de??ildir?",
        L"Akadlar",
        L"Urartular",
        L"Frigler",
        L"Hititler",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangisi M??s??r uygarl??????na ait de??ildir?",
        L"Ziggurat",
        L"Tanr?? Kral",
        L"Firavun",
        L"Hiyeroglif",
        Answer::A)
    );
    this->questions.push_back(std::make_unique<Question>(
        L"Hangi uygarl??k hukuk kurallar??n?? ilk kez yaz??l?? h??le getirmi??tir?",
        L"S??merler",
        L"Babiller",
        L"Asurlar",
        L"Persler",
        Answer::A)
    );

    //std::cout << this->questions.size() << " questions." << std::endl;
}

void MainWindow::selectQuestion(std::size_t index)
{
    this->questions[index]->shuffleAnswers(25);
    this->questionField.setString(this->questions[index]->getQuestion());
    this->optionA.setString(L"A) " + this->questions[index]->getAnswer(Answer::A));
    this->optionB.setString(L"B) " + this->questions[index]->getAnswer(Answer::B));
    this->optionC.setString(L"C) " + this->questions[index]->getAnswer(Answer::C));
    this->optionD.setString(L"D) " + this->questions[index]->getAnswer(Answer::D));
}

void MainWindow::pickedRight() 
{
    score += 2;
    scoreText.setString("Skorun: " + std::to_string(score));
    startGrading(true);
}

void MainWindow::pickedWrong() 
{
    score --;
    scoreText.setString("Skorun: " + std::to_string(score));
    startGrading(false);
}

void MainWindow::saveHighscore() 
{
    std::ofstream ofs(filePath, std::ios::binary);
    ofs.write((char*)&highScore, sizeof(std::int64_t));
    ofs.close();
}

std::size_t MainWindow::generateRandomIndex() 
{
    std::random_device device;
    std::mt19937 gen(device());
    std::uniform_int_distribution<std::size_t> dist(0, questions.size() - 1);
    return dist(gen);
}

void MainWindow::createScoreText() 
{
    scoreText.setString("Skorun: 0");
    scoreText.setFont(font);
    scoreText.setCharacterSize(18);
}

void MainWindow::gradeFunction() 
{
    if(isGrading)
    {
        if(!isBackwards)
        {
            distanceGraded += gradingSpeed;
            if(distanceGraded >= 1)
            {
                distanceGraded = 1;
                isBackwards = true;
            }
            backgroundColor = colorGradiant(originalBackgroundColor, targetColor, distanceGraded);
        }
        else
        {
            distanceGraded -= gradingSpeed;
            if(distanceGraded <= 0)
            {
                distanceGraded = 0;
                isBackwards = false;
                isGrading = false;
            }
            backgroundColor = colorGradiant(originalBackgroundColor, targetColor, distanceGraded);
        }
    }
}

void MainWindow::startGrading(bool toGreen) 
{
    isGrading = true;
    distanceGraded = false;
    isBackwards = false;
    if(toGreen) targetColor = color_right;
    else targetColor = color_wrong;
}
