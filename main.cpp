#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <list>    
#include <cstdlib> 
#include <sstream>
using namespace sf;
using namespace std;




class Bullet {
public:
    RectangleShape shape;
    Vector2f velocity;

    Bullet(float startX, float startY, const Vector2f& velocity) {
        shape.setSize(Vector2f(13.f, 4.f));
        shape.setPosition(startX, startY);
        shape.setFillColor(Color(160, 32, 240));
        this->velocity = velocity;
    }

    void update(float deltaTime) {
        shape.move(velocity * deltaTime);
        if (shape.getPosition().x < 0 || shape.getPosition().x > 1920) {
            deactivate();
        }
    }

    bool checkCollision(const FloatRect& enemyBounds) {
        return shape.getGlobalBounds().intersects(enemyBounds);
    }

    void deactivate() {
        shape.setPosition(-100, -100);
    }

    FloatRect getBounds() const {
        return shape.getGlobalBounds();
    }
};

class Enemy {
public:
    Sprite sprite;
    float speed;
    bool fromRight;

    Enemy(const Texture& texture, float startX, float startY, float speed, bool fromRight) {
        this->sprite.setTexture(texture);
        this->sprite.setPosition(startX, startY);
        this->speed = speed;
        this->fromRight = fromRight;

        if (!fromRight) {
            this->sprite.setScale(-0.3f, 0.3f);  
        } else {
            this->sprite.setScale(0.3f, 0.3f);  
        }
    }                     

    void move(float deltaTime) {
        if (fromRight) {
            sprite.move(-speed * deltaTime, 0);
        } else {
            sprite.move(speed * deltaTime, 0);
        }
    }

    bool isOutOfBounds(int width) {
        if (fromRight) {
            return sprite.getPosition().x + sprite.getGlobalBounds().width < 0;
        } else {
            return sprite.getPosition().x > width;
        }
    }

    FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};

int main() {
    int width = 1920, height = 1080;
    RenderWindow window(VideoMode(width, height), "D I S S E N T");
    window.setFramerateLimit(60);

    SoundBuffer bgSoundBuffer;
    SoundBuffer hitSoundBuffer;
    SoundBuffer shotSoundBuffer;
    SoundBuffer zhitSoundBuffer;
    SoundBuffer gameEndSoundBuffer;

    Sound bgSound;
    Sound hitSound;
    Sound shotSound;
    Sound zhitSound;
    Sound gameEndSound;

    if (!bgSoundBuffer.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\sounds\\bg.ogg")) {
        cerr << "Error loading bg.ogg" << endl;
        return -1;
    }
    if (!hitSoundBuffer.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\sounds\\hit.ogg")) {
        cerr << "Error loading hit.ogg" << endl;
        return -1;
    }
    if (!shotSoundBuffer.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\sounds\\shot.ogg")) {
        cerr << "Error loading shot.ogg" << endl;
        return -1;
    }
    if (!zhitSoundBuffer.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\sounds\\zhit.ogg")) {
        cerr << "Error loading zhit.ogg" << endl;
        return -1;
    }
    if (!gameEndSoundBuffer.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\sounds\\end.ogg")) {
        cerr << "Error loading game_end.ogg" << endl;
        return -1;
    }

    bgSound.setBuffer(bgSoundBuffer);
    hitSound.setBuffer(hitSoundBuffer);
    shotSound.setBuffer(shotSoundBuffer);
    zhitSound.setBuffer(zhitSoundBuffer);
    gameEndSound.setBuffer(gameEndSoundBuffer);

    vector<Bullet> bullets;

    Font font;
    if (!font.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\font\\sansation.TTF")) {
        cerr << "Error loading font" << endl;
        return -1;
    }
    Font rules;
    if (!rules.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\font\\STORMBREAKER.TTF")) {
        cerr << "Error loading rules" << endl;
        return -1;
    }

    Text startText;
    startText.setFont(font);
    startText.setString("Let The DISSENT Begin.");
    startText.setCharacterSize(50);
    startText.setFillColor(Color::White);
    startText.setPosition((width - startText.getLocalBounds().width) / 2, height / 2 - 50);
    
    Text ruletext;
    ruletext.setFont(rules);
    ruletext.setString(
    "RULES OF DISSENT:\n\n"
    "- KILL ALL ZOMBIES.\n"
    "- ONE ZOMBIE KILL -> 1 POINT.\n"
    "- ONE COLLISION WITH A ZOMBIE COSTS YOU ONE LIFE.\n"
    "- A ZOMBIE COMPLETING ONE PASS COSTS 0.25 OF YOUR LIFE.\n"
    "- PRESS 'F' FOR FIRE."
);
    ruletext.setCharacterSize(25);
    ruletext.setFillColor(Color(255, 0, 0));
    ruletext.setPosition((width - ruletext.getLocalBounds().width) / 2.35, height / 1.8 );
    
    int score = 0;

    bool gameStarted = false;
    bool gameEnded = false;
    int collisionCount = 0;
    const int maxCollisions = 4;

    Texture textureLeft, textureRight;
    if (!textureLeft.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\ourchar\\Walk - fromx.png")) {
        cerr << "Error loading Walk - fromx.png" << endl;
        return -1;
    }
    if (!textureRight.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\ourchar\\Walk-from-x.png")) {
        cerr << "Error loading Walk-from-x.png" << endl;
        return -1;
    }

    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\background\\pic.jpg")) {
        cerr << "Error loading background image" << endl;
        return -1;
    }
    Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale(2.2f, 2.2f);
    float bgWidth = backgroundSprite.getGlobalBounds().width;
    float bgHeight = backgroundSprite.getGlobalBounds().height;
    backgroundSprite.setPosition((width - bgWidth) / 2 - 60, (height - bgHeight) / 2);

    Sprite player(textureRight);
    player.setPosition(375.0f, 275.0f);
    int frameWidth = 128;
    int frameHeight = 128;
    player.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
    player.setScale(2.0f, 2.0f);

    float playerSpeed = 500.0f;

    Texture enemyTexture;
    if (!enemyTexture.loadFromFile("C:\\Users\\user\\Desktop\\project\\assets\\enemy\\right.png")) {
        cerr << "Error loading enemy texture (right)" << endl;
        return -1;
    }

    list<Enemy> enemies;
    float spawnTimer = -1.0f;
    float spawnInterval = 1.0f;

    Clock clock;

   
    Color backgroundColor = Color::Black;
    float lives = 4;
    Text livesText;
    livesText.setFont(font);
    livesText.setCharacterSize(30);
    livesText.setFillColor(Color::White);
    livesText.setPosition(10.f, 10.f);

    
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
            }

            if (!gameStarted && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (startText.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
                    gameStarted = true;
                    collisionCount = 0;
                    score = 0;
                    lives = 4;
                    backgroundColor = Color::Black;
                    bgSound.play();
                }
            }

            if (gameEnded && Keyboard::isKeyPressed(Keyboard::Return)) {
                gameStarted = false;
                gameEnded = false;
                gameEndSound.stop();
                hitSound.stop();
                //bgSound.play();
                collisionCount = 0;
                lives = 4;
                backgroundColor = Color::Black;
                enemies.clear();
            }

          if(event.type==sf::Event::KeyReleased){
        		if (event.key.code == sf::Keyboard::F && !gameEnded) {
            		sf::Vector2f bulletDirection = (player.getTexture() == &textureRight) ? sf::Vector2f(1.0f, 0.0f) : sf::Vector2f(-1.0f, 0.0f);
           			Bullet newBullet(player.getPosition().x + frameWidth / 1.3, player.getPosition().y + frameHeight / 0.8f, bulletDirection * 800.0f);
					shotSound.play();
            		bullets.push_back(newBullet);
        		}
			}
        }

        if (!gameStarted) {
            window.clear(Color::Black);
            window.draw(backgroundSprite);  
            window.draw(startText);
            window.draw(ruletext);
            window.display();
            continue;  
        }

        float deltaTime = clock.restart().asSeconds();
        if (gameStarted) {
            if (bgSound.getStatus() != Sound::Playing) {
                bgSound.play();
            }
        }

        if (gameEnded) {
            if (gameEndSound.getStatus() != Sound::Playing) {
                gameEndSound.play();
            }
        }

        Vector2f position = player.getPosition();
        if (Keyboard::isKeyPressed(Keyboard::A) && position.x > 0) {
            player.setTexture(textureLeft);
            player.move(-playerSpeed * deltaTime, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::D) && position.x < width - frameWidth * 2) {
            player.setTexture(textureRight);
            player.move(playerSpeed * deltaTime, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::W) && position.y > 0) {
            player.move(0, -playerSpeed * deltaTime);
        }
        if (Keyboard::isKeyPressed(Keyboard::S) && position.y < height - frameHeight * 2) {
            player.move(0, playerSpeed * deltaTime);
        }

        
        spawnTimer += deltaTime;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.0f;
            float minY = height * 0.15f;
            float maxY = height * 0.85f;
            float startY = rand() % static_cast<int>(maxY - minY + 1) + minY;
            bool fromRight = rand() % 2 == 0;
            if (fromRight) {
                enemies.push_back(Enemy(enemyTexture, width, startY, 400.0f, true));
            } else {
                enemies.push_back(Enemy(enemyTexture, 0, startY, 400.0f, false));
            }
        }

        
        for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
            it->move(deltaTime);
            if (it->isOutOfBounds(width)) {
                collisionCount += 0.25f;
                lives -= 0.25f;
                
                it = enemies.erase(it);
			
                break;  
            }
        }

        
 for (size_t i = 0; i < bullets.size(); ++i) {
    bullets[i].update(deltaTime);

    
    for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
                if (bullets[i].checkCollision(it->getBounds()))  {
            zhitSound.play();
            score++;                                                                                                                                                              
             
            
            it = enemies.erase(it);
			if (i < bullets.size()) {
			    bullets.erase(bullets.begin() + i);
			    --i;  // Adjust index to account for removal
			}

//            bullets.erase(bullets.begin() + i); 
//            --i;
            break;
        }
    }

    if (bullets[i].getBounds().left < 0 || bullets[i].getBounds().left > width) {
		if (i < bullets.size()) {
		    bullets.erase(bullets.begin() + i);
		    --i;  // Adjust index to account for removal
		}

//        bullets.erase(bullets.begin() + i);
//        --i;
    }
}


        
for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
    sf::FloatRect playerBounds = player.getGlobalBounds();
    playerBounds.left += 95;  
    playerBounds.top += 95;   
    playerBounds.width -= 135; 
    playerBounds.height -= 135; 

    if (playerBounds.intersects(it->getBounds())) { 
        hitSound.setPlayingOffset(sf::seconds(0.25f));
        hitSound.play();

        collisionCount++;
        lives--;

        it = enemies.erase(it);  
        if (lives <= 0 || collisionCount >= maxCollisions) {
            gameEnded = true;
            
            bgSound.stop();
            hitSound.stop();
            break; 
        }
    }
}

        ostringstream livesStream;
        livesStream << "Lives: " << lives;
        livesText.setString(livesStream.str());

       
        if (lives <= 0) {
            gameEnded = true;
            
        }

        Text endText;
        endText.setFont(font);
        ostringstream oss;
        oss << "THE END.\n\nScore: " << score;
        endText.setString(oss.str());
        endText.setCharacterSize(100);
        endText.setFillColor(Color::Red);
        endText.setPosition((width - endText.getLocalBounds().width) / 2, height / 2 - 100*2);

        if (collisionCount >= maxCollisions) {
            gameEnded = true;
            
        }

        if (gameEnded) {
            window.clear(Color::Black);  
            window.draw(endText);
            window.display();
            bgSound.stop();
            continue;  
        }

        
        if (!gameEnded) {
            window.clear(backgroundColor);
            window.draw(backgroundSprite);  

            for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
                window.draw(it->sprite);
            }

          for (size_t i = 0; i < bullets.size(); ++i) {
                window.draw(bullets[i].shape);
            }

            
            window.draw(player);

            window.draw(livesText);
        }

        window.display();
    }

    return 0;
}

