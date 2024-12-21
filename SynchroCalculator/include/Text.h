#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <SFML/Window.hpp>

class Text
{
public:
	Text() = default;
	Text(float x, float y, const sf::Font& font, const int& fontSize, const std::string& strText, sf::Color textColor, sf::Color backgroundColor = sf::Color::Transparent)
	{
		text.setFont(font);
		text.setCharacterSize(fontSize);
		text.setFillColor(textColor);
		text.setPosition(x, y);
		background.setPosition(x-2, y);
		background.setFillColor(backgroundColor);
		
		setText(strText);
	}
	void draw(sf::RenderWindow& window)
	{
		window.draw(background);
		window.draw(text);
	}
	void setText(const std::string& str)
{
    text.setString(str);

    // Retrieve the text's font to calculate accurate bounds
    const sf::Font* font = text.getFont();
    if (font != nullptr)
    {
        float textHeight = font->getLineSpacing(text.getCharacterSize());
        float padding = 5.0f;

        // Update the background size
        background.setSize(sf::Vector2f(text.getGlobalBounds().width + padding, textHeight));
    }
}

	std::string getText() const
	{
		return text.getString();
	}
	void setPosition(float x, float y)
	{
		text.setPosition(x, y);
	}
	void setFillColor(sf::Color color)
	{
		text.setFillColor(color);
	}
	void setCharacterSize(int size)
	{
		text.setCharacterSize(size);
	}
	void setStyle(sf::Text::Style style)
	{
		text.setStyle(style);
	}
	sf::FloatRect getGlobalBounds() const
	{
		return text.getGlobalBounds();
	};
private:
	sf::Text text;
	sf::RectangleShape background;
};