#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <windows.h>
#include "Button.h"
#include "Text.h"
#include "Assets/background.h"
#include "Assets/AssetsToBinary.h"
#include "Assets/Roboto.h"
#include "Assets/icon.h"

namespace fs = std::filesystem;

class SynchroCalculator
{
public:
	SynchroCalculator() {};
	int Init();
	~SynchroCalculator() {};
private:
	int sizeX = 400;
	int sizeY = 300;

	const int maxColumns = 8;
    int colCount = 0, rowCount = 0;
	int squareSize = 75;

	int Run(sf::RenderWindow* window);
	int LoadAssets();
	//int SetupText(Text* text, int x, int y);
	int CalculateDamage();
	int BaseDamage();
	void EnterText(sf::Event event, sf::Window* window);
	void EventHandler(sf::Event event, sf::Window* window);
	void SelectText(Text* tooltipText, Text* text);
	void UnselectText();
	void ToggleStayOnTop(sf::RenderWindow& window);

	int damageMultiplier = -1;
	float baseDamge = 207.f;

	int baseResistance = 0;
	int resistancePercentage = 0;

	std::string assetFolder = "Assets";
    std::vector<std::shared_ptr<sf::Texture>> textures;
    std::vector<sf::Sprite> sprites;
	std::vector<bool> selectedStates;

	Text text;

	Text fixedResistanceText;
	Text fixedResistanceInput;

	Text percentResistanceText;
	Text percentResistanceInput;

	Text* selectedText = nullptr;
	sf::Font font;

	sf::Texture backgroundTexture;
	sf::Sprite background;

	Button stayOnTopButton;
	bool stayOnTop = false;

	bool needsUpdate = false;
};