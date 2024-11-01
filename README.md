# Automated Cocktail Machine

## Overview

The Automated Cocktail Machine is a first-year IoT hardware project designed to streamline the cocktail-making process. Users can select their preferred cocktail from a menu or customize their drink using a 4x4 keypad, with selections displayed on a 20x4 LCD screen. The machine includes safety checks for glass detection and ensures the correct positioning of the drink tray with limit switches. Errors such as tray misalignment and absence of a glass are indicated through visual and auditory alerts using the LCD and a buzzer. Additionally, the system supports real-time mixing and serves cocktails with precision, creating an enjoyable user experience.

## Hardware Components

- Arduino Mega
- NEMA 17 stepper motors
- DC motor
- Ultrasonic and IR sensors
- CNC motor shield
- 20x4 LCD screen with I2C module
- 4x4 keypad
- DC motor with A4988 chip
- Limit switch

## Key Features

- **Customized Cocktails**: Customers can enjoy a fully customized cocktail or choose from a saved menu.
- **Mixed Cocktail Option**: Create mixed cocktails based on customer preferences.
- **Real-time Display**: Bottle names, unit prices, and total costs are displayed on the LCD screen.
- **Emergency Stop**: A safety feature allows customers to cancel the order at any time.
- **Precision Positioning**: Limit switches ensure the tray is positioned correctly, with alerts if the tray is not at the starting point.
- **Glass Detection**: Two IR sensors detect the presence of a glass on the tray; if absent, an alert prompts the user to place the glass.
- **Auto-Positioning**: The system automatically moves the tray to the starting point if not correctly positioned.
- **Completion Notification**: Once the drink is ready, a notification appears on the LCD, and a beep sounds.
- **Return to Start**: The tray returns to its initial position once the glass is removed.

## Future Enhancements

- **User Authentication**: Implement a user authentication system for personalized drink preferences.
- **Mobile App Integration**: Develop a mobile application for remote ordering and monitoring.
- **Recipe Suggestions**: Add a feature to suggest cocktail recipes based on available ingredients.
- **Inventory Management**: Track ingredient levels and notify users when supplies are low.
- **Drink Customization Interface**: Create a more interactive LCD interface for easier drink customization.
- **Social Sharing**: Enable users to share their custom cocktails on social media platforms.

## License

This project is licensed under the MIT License.

Copyright (c) 2024 Savindu Amalka


