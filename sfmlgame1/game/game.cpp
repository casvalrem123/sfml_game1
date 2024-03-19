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
    for (TextClass* textMessage : textMessages) {
        delete textMessage;
        textMessage = nullptr;
    }
    textMessages.clear();
    
    delete playerSprite;
    playerSprite = nullptr;
}

void GameManager::runGame() {
    createAssets();
    while (window.isOpen()) {
        createMoreAssets();
        countTime();
        handleEventInput();
        handleGameEvents();
        moveSprites();
        draw();
    }
}

void GameManager::createAssets( ){
    //create assets here
    TextClass* playerWin = new TextClass(sf::Vector2f{0.0f, 0.0f}, 100, sf::Color::White, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/fonts/arial.ttf", "Player wins! time elapsed: ");
    playerWin->setVisibleState(false);
    textMessages.push_back(playerWin);
    
    TextClass* playerLose = new TextClass(sf::Vector2f{0.0f, 0.0f}, 100, sf::Color::White, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/fonts/arial.ttf", "Player lose! time elapsed: ");
    playerLose->setVisibleState(false);
    textMessages.push_back(playerLose);

   for (int i = 0; i< GameComponents.enemyNum; i++){
        Enemy* enemy = new Enemy( sf::Vector2f{
            static_cast<float>(GameComponents.screenWidth - 400),
            static_cast<float>(rand() % GameComponents.screenHeight)
        }, sf::Vector2f{0.6,0.6}, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/sprites/3.png");
        enemySprite.push_back(enemy);
  }
    
    playerSprite = new Player(sf::Vector2f{0.0f, 0.0f}, sf::Vector2f{0.2,0.2}, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/sprites/1.png");
}

void GameManager::createMoreAssets( ){
    if(FlagEvents.mouseClicked){
        Bullet* bullet = new Bullet(playerSprite->getSpritePos(),sf::Vector2f{0.03,0.03}, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/sprites/2.png");
        bullets.push_back(bullet);
    }if(GameEvents.playerWin || GameEvents.playerDead){
        TextClass* timeElapsed = new TextClass(sf::Vector2f{0.0f, 0.0f}, 100, sf::Color::White, "/Users/student/projects/sfmlgame1/sfmlgame1/assets/fonts/arial.ttf", std::to_string(GameComponents.globalTime));
        timeElapsed->setVisibleState(false);
        textMessages.push_back(timeElapsed);
    }
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

void GameManager::handleGameEvents(){
    if(GameEvents.playerDead){
        textMessages[1]->setVisibleState(true);
        textMessages[2]->setVisibleState(true);
        freezeSprites();
    }
    else if(GameEvents.playerWin){
        textMessages[0]->setVisibleState(true);
        textMessages[2]->setVisibleState(true);
        freezeSprites();
    }
}

void GameManager::freezeSprites(){
    playerSprite->setMoveState(false);
    for (Enemy* enemy : enemySprite) {
        enemy->setMoveState(false);
    }
    for (Bullet* bullet : bullets) {
        bullet->setMoveState(false);
    }
}

void GameManager::countTime(){
    sf::Time frameTime = clock.restart();
    GameComponents.deltaTime = frameTime.asSeconds();
    GameComponents.globalTime += frameTime.asSeconds();
}

void GameManager::moveSprites() {
    for (Enemy* enemy : enemySprite) {
        if(enemy->getMoveState())
            enemy->moveEnemy(playerSprite->getSpritePos());
    }
    for (Bullet* bullet : bullets) {
        if(bullet->getMoveState())
            bullet->moveBullet();
    }
    if(playerSprite->getMoveState())
        playerSprite->movePlayer();
}

void GameManager::draw() {
    window.clear();
    window.draw(playerSprite->returnSpritesShape());
    for (TextClass* text : textMessages) {
        if(text->getVisibleState())
        window.draw(*text->getText());
    }
    for (Enemy* enemy : enemySprite) {
        if(enemy->getVisibleState())
        window.draw(enemy->returnSpritesShape());
    }
    for (auto it = bullets.begin(); it != bullets.end();) {
        Bullet* bullet = *it;
        if(bullet->getVisibleState()) {
            window.draw(bullet->returnSpritesShape());
            ++it;
        } else {
            delete bullet;
            it = bullets.erase(it);
        }
    }
    window.display();
}
