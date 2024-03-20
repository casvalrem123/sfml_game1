//
//  game.cpp
//  sfmlgame1
//
//  Created by Sunmyoung Yun on 2024/01/29.
//

#include "game.hpp"
GameManager::GameManager() : window(sf::VideoMode(GameComponents.screenHeight, GameComponents.screenWidth), "sfml game tut") {
    window.setFramerateLimit(30);
}

GameManager::~GameManager() {
    for (Enemy* enemy : enemySprite) {
        delete enemy;
        enemy = nullptr;
    }
    for (Bullet* bullet : bullets) {
        delete bullet;
        bullet = nullptr;
    }
    for (TextClass* text : endMessage){
        delete text;
        text = nullptr;
    }
    delete playerSprite;
    playerSprite = nullptr;
    delete background;
    background = nullptr;
    
    delete backgroundMusic;
    backgroundMusic = nullptr;
    delete bulletSound;
    bulletSound = nullptr;
    delete enemyDeadSound;
    enemyDeadSound = nullptr;
    delete playerDeadSound;
    playerDeadSound = nullptr;
}

void GameManager::runGame() {
    createAssets();
    while (window.isOpen()) {
        if(!GameEvents.gameEnd){
            countTime();
            checkCollision();
            handleGameEvents();
            moveSprites();
        }
        handleEventInput();
        draw();
    }
}

void GameManager::createAssets( ){
   for (int i = 0; i< GameComponents.enemyNum; i++){
        Enemy* enemy = new Enemy( sf::Vector2f{
            static_cast<float>(GameComponents.screenWidth - 900),
            static_cast<float>(rand() % GameComponents.screenHeight)
        }, sf::Vector2f{0.2,0.2}, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/sprites/3.png");
        enemySprite.push_back(enemy);
   }
    playerSprite = new Player(sf::Vector2f{0.0f, 50.0f}, sf::Vector2f{0.2,0.2}, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/sprites/1.png");
    background = new Sprite(sf::Vector2f{0.0f, 0.0f}, sf::Vector2f{1.0,1.0}, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/sprites/4.png");
    backgroundMusic = new MusicClass("/Users/student/projects/sfmlgame1/sfmlgame1/assets/sound/backgroundMusic.ogg");
    backgroundMusic->returnMusic()->play();
    bulletSound = new SoundClass("/Users/student/projects/sfmlgame1/sfmlgame1/assets/sound/bulletSound.wav");
    enemyDeadSound = new SoundClass("/Users/student/projects/sfmlgame1/sfmlgame1/assets/sound/enemyDead.wav");
    playerDeadSound = new SoundClass("/Users/student/projects/sfmlgame1/sfmlgame1/assets/sound/playerDead.wav");
}

void GameManager::handleEventInput(){
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed){
            window.close();
        }
        if (event.type == sf::Event::KeyPressed){
            switch (event.key.code){
                case sf::Keyboard::D:
                    FlagEvents.dPressed = true;
                    break;
                case sf::Keyboard::A:
                    FlagEvents.aPressed = true;
                    break;
                case sf::Keyboard::W:
                    FlagEvents.wPressed = true;
                    break;
                case sf::Keyboard::S:
                    FlagEvents.sPressed = true;
                    break;
                default:
                    break;
            }
        }
        if (event.type == sf::Event::KeyReleased){
            FlagEvents.dPressed = false;
            FlagEvents.aPressed = false;
            FlagEvents.wPressed = false;
            FlagEvents.sPressed = false;
        }
        if (event.type == sf::Event::MouseButtonPressed){
            GameComponents.mouseClickedPos = sf::Mouse::getPosition(window);
            FlagEvents.mouseClicked = true;
        }
        if (event.type == sf::Event::MouseButtonReleased){
            FlagEvents.mouseClicked = false;
        }
    }
}

void GameManager::checkCollision(){
    for (const auto& enemy : enemySprite) {
           sf::FloatRect enemyBounds = enemy->returnSpritesShape().getGlobalBounds();
           if(playerSprite->returnSpritesShape().getGlobalBounds().intersects(enemyBounds)) {
               GameEvents.playerDead = true;
           }
       }
    for (auto bulletIter = bullets.begin(); bulletIter != bullets.end(); ++bulletIter) {
            for (auto enemy : enemySprite) {
                sf::FloatRect bulletBounds = (*bulletIter)->returnSpritesShape().getGlobalBounds();
                sf::FloatRect enemyBounds = enemy->returnSpritesShape().getGlobalBounds();

                if (bulletBounds.intersects(enemyBounds)) {
                    (*bulletIter)->setVisibleState(false);
                    enemy->setVisibleState(false);
                    break;
                }
            }
    }
    if(!enemySprite.size())
        GameEvents.playerWin = true;
}

void GameManager::handleGameEvents(){
    if(FlagEvents.mouseClicked){
        Bullet* bullet = new Bullet(playerSprite->getSpritePos(),sf::Vector2f{0.03,0.03}, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/sprites/2.png");
            bullets.push_back(bullet);
        bulletSound->returnSound()->play();
    }
    
    if(GameEvents.playerWin){
        endingText = "player wins! time elapsed: ";
    }
    else if(GameEvents.playerDead){
        endingText = "player lose! time elapsed: ";
    }
    
    if(GameEvents.playerWin || GameEvents.playerDead){
        endingText.append(std::to_string(GameComponents.globalTime));
        TextClass* endMessage1 = new TextClass(sf::Vector2f{0.0f, 0.0f}, 20, sf::Color::White, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/fonts/arial.ttf", endingText);
            endMessage.push_back(endMessage1);
        GameEvents.gameEnd = true;
        backgroundMusic->returnMusic()->stop();
    }
}

void GameManager::countTime(){
    sf::Time frameTime = clock.restart();
    GameComponents.deltaTime = frameTime.asSeconds();
    GameComponents.globalTime += frameTime.asSeconds();
}

void GameManager::moveSprites() {
    for (Enemy* enemy : enemySprite)
        enemy->moveEnemy(playerSprite->getSpritePos());
    for (Bullet* bullet : bullets)
        bullet->moveBullet();
    playerSprite->movePlayer();
}

void GameManager::draw() {
    window.clear();
    window.draw(background->returnSpritesShape());
    window.draw(playerSprite->returnSpritesShape());
   
    for (TextClass* text : endMessage){
        if(text->getVisibleState())
            window.draw(*text->getText());
    }
    for (auto it = enemySprite.begin(); it != enemySprite.end();) {
        Enemy* enemy = *it;
        if(enemy->getVisibleState()) {
            window.draw(enemy->returnSpritesShape());
            ++it;
        } else {
            delete enemy;
            enemy = nullptr;
            it = enemySprite.erase(it);
        }
    }
    for (auto it = bullets.begin(); it != bullets.end();) {
        Bullet* bullet = *it;
        if(bullet->getVisibleState()) {
            window.draw(bullet->returnSpritesShape());
            ++it;
        } else {
            delete bullet;
            bullet = nullptr;
            it = bullets.erase(it);
        }
    }
    window.display();
}
