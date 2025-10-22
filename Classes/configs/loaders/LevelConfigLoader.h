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
 * �ؿ����ü�����
 * ������ļ�����JSON�����عؿ�����
 * ����Ϊ��̬�࣬����Ҫʵ����
 */
class LevelConfigLoader
{
public:
    // ��̬��������ָ���ؿ�ID������
    static LevelConfig& loadLevelConfig(int levelId)
    {
        // �����ļ�����level_1.json
        std::string filename = StringUtils::format("level_%d.json", levelId);

        // ��ȡ�ļ�����·��
        std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
        if (fullPath.empty()) {
            CCLOG("LevelConfigLoader: file not found: %s", filename.c_str());
            return LevelConfig::getInstance();
        }

        // ��ȡ�ļ�����
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

        // ����������LevelConfig����
        LevelConfig& levelConfig = LevelConfig::getInstance();

        // ��������������
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
            levelConfig.setMainAreaCards(playFieldCards);  // ��������������
        }

        // ����������������
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
                // �����ƶѵ�λ��ͨ���ɳ�����㣬���������Ƕ�ȡ�����е�λ��
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
            levelConfig.setBackupAreaCards(stackCards);  // ���ñ�����������
        }

        return levelConfig;  // ���ع����LevelConfig����
    }
};

#endif // LEVEL_CONFIG_LOADER_H