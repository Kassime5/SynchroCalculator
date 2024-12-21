#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

class Button
{
public:
	Button() = default;
    Button(float x, float y, float width, float height, const sf::Font& font, const int& fontSize, const std::string& text, sf::Color textColor, sf::Color buttonColor, std::function<void()> function)
    {
        // Configure the button shape
        buttonShape.setPosition(x, y);
        buttonShape.setSize(sf::Vector2f(width, height));
        buttonShape.setFillColor(buttonColor);

        // Configure the button text
        buttonText.setFont(font);
        buttonText.setCharacterSize(fontSize);
        buttonText.setFillColor(textColor);

		setText(text);

		onClick = function;
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(buttonShape);
        window.draw(buttonText);
    }

    bool isClicked(const sf::Vector2i& mousePos)
    {
        if (buttonShape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
		{
			onClick();
			return true;
		}
		return false;
    }

    void setColor(sf::Color color)
    {
        buttonShape.setFillColor(color);
    }

	void setText(const std::string& text)
	{
		buttonText.setString(text);
		sf::FloatRect textBounds = buttonText.getLocalBounds();
		buttonText.setPosition(
			buttonShape.getPosition().x + (buttonShape.getSize().x - textBounds.width) / 2.0f - textBounds.left,
			buttonShape.getPosition().y + (buttonShape.getSize().y - textBounds.height) / 2.0f - textBounds.top
		);

	}

private:
    sf::RectangleShape buttonShape;
    sf::Text buttonText;
	std::function<void()> onClick;
};