// // DEPRECATED

// #include <Arduino_JSON.h>
// void debugPrintRawJSON(const String& rawJson) {
//   Serial.println("📦 Raw schedule debug:");

//   int entryCount = 0;
//   int pos = 0;

//   while ((pos = rawJson.indexOf("{", pos)) != -1) {
//     entryCount++;
//     Serial.print("Entry ");
//     Serial.print(entryCount);
//     Serial.println(":");

//     int entryEnd = rawJson.indexOf("}", pos);
//     if (entryEnd == -1) break;

//     String entry = rawJson.substring(pos, entryEnd + 1);

//     // Extract each field
//     int titleIndex = entry.indexOf("\"title\"");
//     if (titleIndex != -1) {
//       int valStart = entry.indexOf(":", titleIndex) + 1;
//       int quote1 = entry.indexOf("\"", valStart);
//       int quote2 = entry.indexOf("\"", quote1 + 1);
//       String title = entry.substring(quote1 + 1, quote2);
//       Serial.print("  Title: ");
//       Serial.println(title);
//     }

//     int startIndex = entry.indexOf("\"start_time\"");
//     if (startIndex != -1) {
//       int valStart = entry.indexOf(":", startIndex) + 1;
//       int quote1 = entry.indexOf("\"", valStart);
//       int quote2 = entry.indexOf("\"", quote1 + 1);
//       String startTime = entry.substring(quote1 + 1, quote2);
//       Serial.print("  Start: ");
//       Serial.println(startTime);
//     }

//     int endIndex = entry.indexOf("\"end_time\"");
//     if (endIndex != -1) {
//       int valStart = entry.indexOf(":", endIndex) + 1;
//       int quote1 = entry.indexOf("\"", valStart);
//       int quote2 = entry.indexOf("\"", quote1 + 1);
//       String endTime = entry.substring(quote1 + 1, quote2);
//       Serial.print("  End: ");
//       Serial.println(endTime);
//     }

//     int typeIndex = entry.indexOf("\"type\"");
//     if (typeIndex != -1) {
//       int valStart = entry.indexOf(":", typeIndex) + 1;
//       int quote1 = entry.indexOf("\"", valStart);
//       int quote2 = entry.indexOf("\"", quote1 + 1);
//       String type = entry.substring(quote1 + 1, quote2);
//       Serial.print("  Type: ");
//       Serial.println(type);
//     }

//     Serial.println();
//     pos = entryEnd + 1;
//   }

//   Serial.print("🔎 Parsed ");
//   Serial.print(entryCount);
//   Serial.println(" entries.");
// }
