# EcoSnake

Welcome to **EcoSnake** — an educational Snake game designed to promote environmental awareness while engaging players in classic Snake gameplay. Developed as part of the project “Gender, Digitalization, Green: Ensuring a Sustainable Future for all in Europe” (Ref. 2023-1-RO01-KA220-HED-000154433), EcoSnake combines fun with learning through eco-themed questions that test and enhance the player’s knowledge of sustainability.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Overview

EcoSnake is built with C++ for core mechanics, leveraging Raylib for 2D rendering and UI design. The game incorporates a series of themed questions that appear at key points, challenging the player’s understanding of eco-friendly practices and sustainable development. The project uses linked lists to manage and dynamically display these questions, creating a seamless blend of gaming and learning.

## Features

- **Classic Snake Gameplay**: Enjoy the familiar Snake game.
- **Eco-Themed Questions**: Answer sustainability-related questions to progress and increase your score.
- **Dynamic Question Management**: Uses linked lists to store and display questions, ensuring efficient performance.
- **Interactive Learning**: Each question aims to educate players on eco-friendly practices, supporting sustainability awareness.

## Installation

### Prerequisites

- **C++ Compiler**: Make sure you have a C++ compiler installed (e.g., g++, clang).
- **Raylib**: Install Raylib for 2D graphics rendering.
  - On Linux: `sudo apt install libraylib-dev`
  - On macOS: `brew install raylib`
  - For Windows, follow the installation instructions on [Raylib's official website](https://www.raylib.com/).

### Clone the Repository

```bash
git clone https://github.com/MarioTimoc21/EcoSnake.git
cd EcoSnake/SnakeEco
```
### Build and Run

1. Compile the project:
    ```bash
    g++ -o EcoSnake main.cpp -lraylib
    ```
   
2. Run the game:
    ```bash
    ./EcoSnake
    ```

## Usage

- **Learning**: Make sure to go through MODULE I. CLIMATE CHANGE, by pressing the Module button in the menu in order to learn about the topic. 
- **Arrow Keys**: Control the direction of the Snake.
- **Answer Questions**: When prompted, answer eco-themed questions to continue.
- **Objective**: Collect points by consuming items and correctly answering questions to grow the Snake and increase your score.

## Project Structure

- **main.cpp**: Core game logic and setup.
- **questions.cpp**: Handles question management, including loading and displaying questions.
- **grid.cpp**: Manages the game grid where the Snake moves.
- **assets/**: Contains any assets, like images or text files, used in the game.

## Contributing

Contributions are welcome! If you’d like to improve EcoSnake, please follow these steps:

1. Fork the repository.
2. Create a new branch: `git checkout -b feature/YourFeature`
3. Commit your changes: `git commit -m 'Add Your Feature'`
4. Push to the branch: `git push origin feature/YourFeature`
5. Open a pull request.

## Acknowledgments

EcoSnake was developed as part of the project “Gender, Digitalization, Green: Ensuring a Sustainable Future for all in Europe” (Ref. 2023-1-RO01-KA220-HED-000154433), with contributions supporting sustainable development and environmental education.

---

Enjoy playing **EcoSnake**, and thank you for supporting sustainability through learning!
