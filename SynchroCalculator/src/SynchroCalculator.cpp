#include "SynchroCalculator.h"

int SynchroCalculator::Init()
{
    sf::RenderWindow window(sf::VideoMode(sizeX, sizeY), "Synchro Calculator", sf::Style::Close);

    // Set layered window style and make it fully opaque initially
    HWND hwnd = window.getSystemHandle();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    window.setFramerateLimit(15);
    squareSize = sizeX / maxColumns;

    if (LoadAssets() != 0)
        return -1;

    fixedResistanceText = Text(10, 50, font, 24, "Fixed resistance: ", sf::Color::White);
    fixedResistanceInput = Text(200, 50, font, 24, std::to_string(baseResistance), sf::Color::White);

    percentResistanceText = Text(10, 90, font, 24, "Percentage resistance: ", sf::Color::White);
    percentResistanceInput = Text(265, 90, font, 24, std::to_string(resistancePercentage), sf::Color::White);

    text = Text(10, 10, font, 24, "Current damage: " + std::to_string(CalculateDamage()), sf::Color::White);

    stayOnTopButton = Button(sizeX - 80, 10, 70, 30, font, 20, "Pin", sf::Color::White, sf::Color::Blue, 
                             std::bind(&SynchroCalculator::ToggleStayOnTop, this, std::ref(window)));

    return Run(&window);
}



int SynchroCalculator::Run(sf::RenderWindow* window)
{
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
			EventHandler(event, window);
        }

        // Clear the window
		window->clear(sf::Color(0, 0, 0, 0));
		window->draw(background);
		stayOnTopButton.draw(*window);

        // Draw all sprites
        for (const auto& sprite : sprites)
        {
            window->draw(sprite);
        }

		text.draw(*window);
		fixedResistanceText.draw(*window);
		fixedResistanceInput.draw(*window);
		percentResistanceText.draw(*window);
		percentResistanceInput.draw(*window);


        // Display the frame
        window->display();
    }

    return 0;
}

void SynchroCalculator::EventHandler(sf::Event event, sf::Window* window)
{
	if (event.type == sf::Event::Closed)
		window->close();
	if (event.type == sf::Event::MouseEntered)
	{
		HWND hwnd = window->getSystemHandle();
		SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	}
	else if (event.type == sf::Event::MouseLeft)
	{
		HWND hwnd = window->getSystemHandle();
		SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 128, LWA_ALPHA);
	}


	if (event.type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
		//std::cout << "Mouse position: " << mousePos.x << " " << mousePos.y << std::endl;
        for (size_t i = 0; i < sprites.size(); ++i)
        {
            if (sprites[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
            {
                // Toggle the selected state
                selectedStates[i] = !selectedStates[i];

                if (selectedStates[i])
                {
                    // Change color if selected
                    sprites[i].setColor(sf::Color(255, 0, 0)); // Red tint
                    damageMultiplier += 2;
					text.setText("Current damage: " + std::to_string(CalculateDamage()));
                }
                else
                {
                    // Reset color if deselected
                    sprites[i].setColor(sf::Color(255, 255, 255)); // Original
                    damageMultiplier -= 2;
					text.setText("Current damage: " + std::to_string(CalculateDamage()));
                }
            }
        }
		if (fixedResistanceInput.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
		{
			SelectText(&fixedResistanceText, &fixedResistanceInput);
		}
		else if (percentResistanceInput.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
		{
			SelectText(&percentResistanceText, &percentResistanceInput);
        }
		stayOnTopButton.isClicked(mousePos);
	}
	if (event.type == sf::Event::TextEntered)
		EnterText(event, window);
}

void SynchroCalculator::SelectText(Text* tooltipText, Text* inputText)
{
	UnselectText();
	selectedText = inputText;
	inputText->setText("");
	tooltipText->setFillColor(sf::Color::Red);
	tooltipText->setStyle(sf::Text::Underlined);
}

void SynchroCalculator::UnselectText()
{
	if (selectedText != nullptr)
	{
		fixedResistanceText.setFillColor(sf::Color::White);
		fixedResistanceText.setStyle(sf::Text::Regular);
		percentResistanceText.setFillColor(sf::Color::White);
		percentResistanceText.setStyle(sf::Text::Regular);
		selectedText = nullptr;
	}
}

void SynchroCalculator::ToggleStayOnTop(sf::RenderWindow& window)
{
	// Get the native handle (HWND) for the SFML window
    HWND hwnd = window.getSystemHandle();
	stayOnTop = !stayOnTop;
	if (stayOnTop)
	{
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		stayOnTopButton.setColor(sf::Color(0,0,139));
		stayOnTopButton.setText("Pinned");
	}
	else
	{
		SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		stayOnTopButton.setColor(sf::Color::Blue);
		stayOnTopButton.setText("Pin");
	}
}

int SynchroCalculator::LoadAssets()
{
	for (const auto& asset : assets) {
		unsigned char* data = asset.first;
		unsigned int size = asset.second;

		auto texture = std::make_shared<sf::Texture>();
		if (texture->loadFromMemory(data, size)) {
			textures.push_back(texture);

			sf::Sprite sprite;
			sprite.setTexture(*texture);
			sprite.setScale(static_cast<float>(squareSize) / texture->getSize().x, static_cast<float>(squareSize) / texture->getSize().y);
			sprites.push_back(sprite);

			// Initialize selected state
			selectedStates.push_back(false);
		} 
		else {
			std::cerr << "Failed to load texture from memory!" << std::endl;
		}
	}	


	int totalSprites = sprites.size();
	int rowCount = (totalSprites + maxColumns - 1) / maxColumns;
	int rowPos = sizeY - (rowCount * squareSize);
	for (auto& sprite : sprites)
	{
		sprite.setPosition(colCount * squareSize, rowPos);
		colCount++;
		if (colCount >= maxColumns)
		{
			colCount = 0;
			rowPos += squareSize;
		}
	}

	if (!backgroundTexture.loadFromMemory(background_png, background_png_len)) {
		std::cerr << "Failed to load texture from memory!" << std::endl;
	}
	background.setTexture(backgroundTexture);
	background.setScale(static_cast<float>(sizeX) / backgroundTexture.getSize().x, static_cast<float>(sizeY) / backgroundTexture.getSize().y);
    background.setColor(sf::Color(255, 255, 255, 100));

	if (!font.loadFromMemory(Roboto_ttf, Roboto_ttf_len))
	{
		std::cerr << "Failed to load font." << std::endl;
		return -1;
	}
	font = sf::Font(font);

	return 0;
}

int SynchroCalculator::CalculateDamage()
{
	int result = (int)BaseDamage() * (100 - resistancePercentage) / 100;
    result = result * damageMultiplier;
	if (result < 0 || damageMultiplier < 0)
	{
		return 0;
	}
    return (int)result;
}

int SynchroCalculator::BaseDamage()
{
	return baseDamge - baseResistance < 0 ? 0 : baseDamge - baseResistance;
}

void SynchroCalculator::EnterText(sf::Event event, sf::Window* window)
{
	if (selectedText == nullptr)
	{
		return;
	}
	// If the text entered is a number, add it to the string
	if (event.text.unicode >= 48 && event.text.unicode <= 57)
	{
		selectedText->setText(selectedText->getText() + static_cast<char>(event.text.unicode));
	}
	// If the backspace key is pressed, remove the last character
	else if (event.text.unicode == 8)
	{
		std::string currentString = selectedText->getText();
		if (currentString.size() > 0)
		{
			currentString.pop_back();
			selectedText->setText(currentString);
		}
	}
	// If a "-" is entered, add it to the string if it's the first character
	else if (event.text.unicode == 45)
	{
		if (selectedText->getText() == "")
		{
			selectedText->setText("-");
		}
	}
	// If the enter key is pressed, set the selected text to nullptr
	else if (event.text.unicode == 13)
	{
		// If the text is empty, set it to 0
		if (selectedText->getText() == "")
		{
			selectedText->setText("0");
		}

		// Converts the input into number to add to their corresponding variables
		baseResistance = std::stoi(fixedResistanceInput.getText());
		resistancePercentage = std::stoi(percentResistanceInput.getText());
		// Recalculate the damage
		text.setText("Current damage: " + std::to_string(CalculateDamage()));
		UnselectText();
	}
}