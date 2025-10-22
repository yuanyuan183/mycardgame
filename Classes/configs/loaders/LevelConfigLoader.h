#pragma once
#ifndef LEVEL_CONFIG_LOADER_H
#define LEVEL_CONFIG_LOADER_H


#include "../models/LevelConfig.h"
#include "cocos2d.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

USING_NS_CC;

/**
 * 关卡配置加载器
 * 负责从文件（如JSON）加载关卡配置
 * 该类为静态类，不需要实例化
 */
class LevelConfigLoader
{
public:
    // 静态方法加载指定关卡ID的配置
    static LevelConfig& loadLevelConfig(int levelId)
    {
        // 构建文件名，level_1.json
        std::string filename = StringUtils::format("level_%d.json", levelId);

        // 获取文件完整路径
        std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
        if (fullPath.empty()) {
            CCLOG("LevelConfigLoader: file not found: %s", filename.c_str());
            return LevelConfig::getInstance();
        }

        // 读取文件内容
        std::string content = FileUtils::getInstance()->getStringFromFile(fullPath);
        if (content.empty()) {
            CCLOG("LevelConfigLoader: file is empty: %s", filename.c_str());
            return LevelConfig::getInstance();
        }

        rapidjson::Document doc;
        doc.Parse(content.c_str());
        if (doc.HasParseError()) {
            CCLOG("LevelConfigLoader: JSON parse error: %s", doc.GetParseError());
            return LevelConfig::getInstance();
        }

        // 创建并返回LevelConfig对象
        LevelConfig& levelConfig = LevelConfig::getInstance();

        // 解析主牌区卡牌
        if (doc.HasMember("Playfield") && doc["Playfield"].IsArray()) {
            std::vector<LevelConfig::CardItem> playFieldCards;
            const rapidjson::Value& playfieldArray = doc["Playfield"];
            for (rapidjson::SizeType i = 0; i < playfieldArray.Size(); i++) {
                const rapidjson::Value& cardObj = playfieldArray[i];
                LevelConfig::CardItem cardConfig;
                if (cardObj.HasMember("CardFace") && cardObj["CardFace"].IsInt()) {
                    cardConfig.cardValue = static_cast<CardFaceType>(cardObj["CardFace"].GetInt());
                }
                if (cardObj.HasMember("CardSuit") && cardObj["CardSuit"].IsInt()) {
                    cardConfig.cardSuit = static_cast<CardSuitType>(cardObj["CardSuit"].GetInt());
                }
                if (cardObj.HasMember("Position") && cardObj["Position"].IsObject()) {
                    const rapidjson::Value& posObj = cardObj["Position"];
                    if (posObj.HasMember("x") && posObj["x"].IsNumber() &&
                        posObj.HasMember("y") && posObj["y"].IsNumber()) {
                        cardConfig.cardPosition.x = posObj["x"].GetFloat();
                        cardConfig.cardPosition.y = posObj["y"].GetFloat();
                    }
                }
                playFieldCards.push_back(cardConfig);
            }
            levelConfig.setMainAreaCards(playFieldCards);  // 设置主牌区卡牌
        }

        // 解析备用牌区卡牌
        if (doc.HasMember("Stack") && doc["Stack"].IsArray()) {
            std::vector<LevelConfig::CardItem> stackCards;
            const rapidjson::Value& stackArray = doc["Stack"];
            for (rapidjson::SizeType i = 0; i < stackArray.Size(); i++) {
                const rapidjson::Value& cardObj = stackArray[i];
                LevelConfig::CardItem cardConfig;
                if (cardObj.HasMember("CardFace") && cardObj["CardFace"].IsInt()) {
                    cardConfig.cardValue = static_cast<CardFaceType>(cardObj["CardFace"].GetInt());
                }
                if (cardObj.HasMember("CardSuit") && cardObj["CardSuit"].IsInt()) {
                    cardConfig.cardSuit = static_cast<CardSuitType>(cardObj["CardSuit"].GetInt());
                }
                // 备用牌堆的位置通常由程序计算，但这里我们读取配置中的位置
                if (cardObj.HasMember("Position") && cardObj["Position"].IsObject()) {
                    const rapidjson::Value& posObj = cardObj["Position"];
                    if (posObj.HasMember("x") && posObj["x"].IsNumber() &&
                        posObj.HasMember("y") && posObj["y"].IsNumber()) {
                        cardConfig.cardPosition.x = posObj["x"].GetFloat();
                        cardConfig.cardPosition.y = posObj["y"].GetFloat();
                    }
                }
                stackCards.push_back(cardConfig);
            }
            levelConfig.setBackupAreaCards(stackCards);  // 设置备用牌区卡牌
        }

        return levelConfig;  // 返回构造的LevelConfig对象
    }
};

#endif // LEVEL_CONFIG_LOADER_H