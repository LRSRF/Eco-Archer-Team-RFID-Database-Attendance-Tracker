# [Eco Archer Team RFID Database Attendance Tracker]

The RFID Database & Attendance Tracker is designed to automate attendance monitoring at the Eco Archer Team Office. Team members will be able to use their RFID-enabled school ID cards to record time-in and time-out. The system automatically logs attendance in a Google Sheet for record-keeping and sends real-time notifications to the Eco Archer Team Discord server, allowing members and admins to view attendance events instantly. Unauthorized IDs attempting access are also flagged and reported.

## System Overview

- **Hardware:** ESP32, MFRC522 RFID Reader, SSD1306 OLED Display, Buzzer
- **Backend:** Google Apps Script + Google Sheets
- **IDE:** PlatformIO IDE
- **Communication:** HTTPS API calls
- **Notifications:** Discord Webhook Integration

## Documentation

[RFID Database & Attendance Tracker Documentation](https://docs.google.com/document/d/1TlxIlPTxwVNUh1epnYhAwK3cgbeKFJYe4rPi2grezOs/edit?usp=sharing)

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE - see the [LICENSE](https://github.com/LRSRF/-Eco-Archer-Team-RFID-Database-Attendance-Tracker/blob/main/LICENSE) file for details.
