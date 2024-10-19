#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <exprtk.hpp>
#include <cmath>

class FunctionPlotter {
public:
    FunctionPlotter() : offsetX(400.0f), offsetY(300.0f), scale(50.0f) { 
        if (!font.loadFromFile("resources/flanker.otf")) {
            std::cerr << "Error loading font" << std::endl;
        }

        inputText.setFont(font);
        inputText.setCharacterSize(24);
        inputText.setFillColor(sf::Color::Black); 
        inputText.setPosition(10.f, 10.f);

        // set up viewport
        view.setSize(800, 600);
        view.setCenter(400, 300);  
    }

    void run() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "funcy plotta");
        std::string userInput = "";
		initInputText();

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                handleInput(event, userInput, window);
            }

            window.clear(sf::Color::White);  // set backdrop to white
            window.setView(view);

			// draw grid and function
            drawGrid(window);
            drawFunction(window, userInput);

            window.setView(window.getDefaultView()); // reset view to draw ui elements
            window.draw(inputText);
            window.display();
        }
    }

private:
    sf::Font font;
    sf::Text inputText;
    sf::View view;
    float offsetX, offsetY, scale;

	void initInputText() {
		inputText.setString("f(x) = ");
	}

    // handle inputs and movement
    void handleInput(sf::Event& event, std::string& userInput, sf::RenderWindow& window) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b' && !userInput.empty()) {
                userInput.pop_back();
            }
            else if (event.text.unicode < 128) {
                userInput += static_cast<char>(event.text.unicode);
            }
            inputText.setString("f(x) = " + userInput);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            view.move(0, -10);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            view.move(0, 10);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            view.move(-10, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            view.move(10, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp)) {
            scale *= 1.1f;  // Zoom in
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown)) {
            scale /= 1.1f;  // Zoom out
        }
    }

	// draw grid lines
    void drawGrid(sf::RenderWindow& window) {
        sf::VertexArray gridLines(sf::Lines);

        // get current viewport params
        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewSize = view.getSize();

		// calculate the boundaries of the view
        float left = viewCenter.x - viewSize.x / 2;
        float right = viewCenter.x + viewSize.x / 2;
        float top = viewCenter.y - viewSize.y / 2;
        float bottom = viewCenter.y + viewSize.y / 2;

        // step size for grid
        float step = scale;

        // draw vertical grid lines
        for (float x = std::floor(left / step) * step; x <= right; x += step) {
            gridLines.append(sf::Vertex(sf::Vector2f(x, top), sf::Color(200, 200, 200)));  // Light gray grid lines
            gridLines.append(sf::Vertex(sf::Vector2f(x, bottom), sf::Color(200, 200, 200)));
        }

        // draw horizontal grid lines
        for (float y = std::floor(top / step) * step; y <= bottom; y += step) {
            gridLines.append(sf::Vertex(sf::Vector2f(left, y), sf::Color(200, 200, 200)));  // Light gray grid lines
            gridLines.append(sf::Vertex(sf::Vector2f(right, y), sf::Color(200, 200, 200)));
        }

        // display to window
        window.draw(gridLines);
    }



	// function to parse math functions and draw them
    void drawFunction(sf::RenderWindow& window, const std::string& userInput) {
        if (userInput.empty()) return;

        exprtk::symbol_table<double> symbolTable;
        double x = 0;
        symbolTable.add_variable("x", x);
        symbolTable.add_constants();

        exprtk::expression<double> expression;
        expression.register_symbol_table(symbolTable);

        exprtk::parser<double> parser;
        if (!parser.compile(userInput, expression)) {
            std::cerr << "error: invalid function" << std::endl;
            return;
        }

        sf::VertexArray lineStrip(sf::LineStrip);

        for (int i = -800; i < 800; ++i) {
            x = i / scale;
            double y = expression.value();

            sf::Vector2f point(i + offsetX, -y * scale + offsetY);
            lineStrip.append(sf::Vertex(point, sf::Color::Red));
        }

        window.draw(lineStrip);
    }
};

int main() {
    FunctionPlotter plotter;
    plotter.run();
    return 0;
}