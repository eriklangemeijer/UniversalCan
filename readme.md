# UniversalCan

UniversalCan is a tool for communicating CAN messages using a configurable message file.

## Purpose

Modern vehicles often require reading and transmitting CAN messages for troubleshooting or configuration. While many programs exist for standard protocols across platforms (Windows, Android, iOS), most vehicles also rely on proprietary manufacturer-specific messages.

Third-party tools address these needs but often come with challenges:

- High costs 
- Limited developer support for updates and bug fixes.
- Developers often lack access to all vehicle models to ensure compatibility.
- Platform restrictions (e.g., Android-only apps).

On the other hand, hobbyists with access to vehicles may analyze protocols but often lack the time or skills to develop comprehensive communication software. When they do make such a program it results in redundant effort as each vehicle-specific tool reinvents the wheel.

## Goal

UniversalCan separates message definitions from communication software, enabling hobbyists to:

- Customize the tool for their vehicle.
- Share protocol definition files with others.

This approach fosters collaboration and reduces duplication of effort.