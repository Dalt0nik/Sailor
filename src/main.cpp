#include <iostream>
#include <fstream>
#include <cpr/cpr.h>
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include "DatabaseManager.h"
#include "TradeRepository.h"
#include "AssetService.h"

// Define GLEW_STATIC and include GLEW header
#define GLEW_STATIC
#include <GL/glew.h>

// ImGui includes
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

// ImGuiDateChooser includes
#include "addons/imguidatechooser/imguidatechooser.h"

using json = nlohmann::json;

// Function to read API key from a secret
std::string readSecrets(const std::string& fileName, const std::string& keyToFind) {
    std::ifstream secretsFile(fileName);
    std::string line;
    std::string value = ""; // Initialize to empty string in case key is not found

    if (secretsFile.is_open()) {
        while (std::getline(secretsFile, line)) {
            // Find the position of '='
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                // Extract the key and value from the line
                std::string key = line.substr(0, equalsPos);
                value = line.substr(equalsPos + 1);

                // Remove leading and trailing whitespaces from key and value
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                // Check if the key matches the keyToFind
                if (key == keyToFind) {
                    break; // Key found, no need to continue searching
                }
            }
        }
        secretsFile.close();
    }
    else {
        std::cout << "Unable to open secrets file" << std::endl;
    }
    return value;
}

struct StockOperationData {
    char ticker[10] = "";
    tm date = {};
    int amount = 0;
    double price = 0.0;
};

struct PortfolioData {
    double total_ticker_value = 0.0;
    double portfolio_value = 0.0;
};

std::string formatDate(const tm& date) {
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &date);
    return std::string(buffer);
}

void renderMenu(AssetService& assetService) {
    static StockOperationData buyData;
    static StockOperationData sellData;
    static StockOperationData stockValueData;
    static PortfolioData portfolioData;
    static char statusMessage[256] = "";

    ImGui::Begin("Sailor App");

    ImGui::Text("Welcome to the Sailor app! We help to multiply your money $");

    if (ImGui::CollapsingHeader("Buy Stock")) {
        ImGui::InputText("Ticker##buy", buyData.ticker, IM_ARRAYSIZE(buyData.ticker));
        ImGui::InputInt("Amount##buy", &buyData.amount);
        ImGui::InputDouble("Price##buy", &buyData.price);
        ImGui::Text("Date:");
        if (ImGui::DateChooser("##buy_date", buyData.date)) {
            // Date selection
        }
        if (ImGui::Button("Buy##buy")) {
            std::string dateStr = formatDate(buyData.date);
            int result = assetService.buy_stock(buyData.ticker, buyData.amount, buyData.price, dateStr);
            if (result == 1) {
                snprintf(statusMessage, sizeof(statusMessage), "Buy operation successful");
            }
            else {
                snprintf(statusMessage, sizeof(statusMessage), "Buy operation failed");
            }
        }
    }

    if (ImGui::CollapsingHeader("Sell Stock")) {
        ImGui::InputText("Ticker##sell", sellData.ticker, IM_ARRAYSIZE(sellData.ticker));
        ImGui::InputInt("Amount##sell", &sellData.amount);
        ImGui::InputDouble("Price##sell", &sellData.price);
        ImGui::Text("Date:");
        if (ImGui::DateChooser("##sell_date", sellData.date)) {
            // Date selection
        }
        if (ImGui::Button("Sell##sell")) {
            std::string dateStr = formatDate(sellData.date);
            int result = assetService.sell_stock(sellData.ticker, sellData.amount, sellData.price, dateStr);
            if (result == 1) {
                snprintf(statusMessage, sizeof(statusMessage), "Sell operation successful");
            }
            else {
                snprintf(statusMessage, sizeof(statusMessage), "Sell operation failed");
            }
        }
    }

    if (ImGui::CollapsingHeader("Check Asset Value")) {
        ImGui::InputText("Ticker##check_asset", stockValueData.ticker, IM_ARRAYSIZE(stockValueData.ticker));
        if (ImGui::Button("Check Value##check_asset")) {
            double value = assetService.get_total_ticker_value(stockValueData.ticker);
            if (value != -1.0) {
                portfolioData.total_ticker_value = value;
                snprintf(statusMessage, sizeof(statusMessage), "Check asset value successful");
            }
            else {
                snprintf(statusMessage, sizeof(statusMessage), "Check asset value failed");
            }
        }
        ImGui::Text("Total Value: %.2f", portfolioData.total_ticker_value);
    }

    if (ImGui::CollapsingHeader("Check Portfolio Value")) {
        if (ImGui::Button("Check Portfolio Value##check_portfolio")) {
            double value = assetService.get_portfolio_value();
            if (value != -1.0) {
                portfolioData.portfolio_value = value;
                snprintf(statusMessage, sizeof(statusMessage), "Check portfolio value successful");
            }
            else {
                snprintf(statusMessage, sizeof(statusMessage), "Check portfolio value failed");
            }
        }
        ImGui::Text("Portfolio Value: %.2f", portfolioData.portfolio_value);
    }

    // Display status message
    if (strlen(statusMessage) > 0) {
        ImGui::Text("%s", statusMessage);
    }

    ImGui::End();
}

int main(int argc, char** argv) {
    // Read the API key
    std::string api_key = readSecrets("secrets.txt", "api_key");

    // Initialize the database
    DatabaseManager dbManager("portfolio.db", "scripts/db-setup.sql", "scripts/mock-data.sql");
    TradeRepository tradeRepository(dbManager);
    AssetService assetService(tradeRepository, api_key);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Sailor App", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render the GUI
        renderMenu(assetService);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
