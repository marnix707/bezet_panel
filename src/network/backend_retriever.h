// // DEPRECATED
// void printNextUpcomingBooking(const JSONVar& schedule) {
//   time_t now = time(nullptr);
//   JSONVar nextEvent;
//   time_t soonest = 0;

//   for (int i = 0; i < schedule.length(); i++) {
//     if (JSON.typeof(schedule[i]) != "object") {
//       Serial.print("Skipping non-object at index ");
//       Serial.println(i);
//       continue;
//     }

//     JSONVar entry = schedule[i];

//     // Check if "start_time" exists and is a string
//     if (!entry.hasOwnProperty("start_time") || JSON.typeof(entry["start_time"]) != "string") {
//       Serial.print("Missing or invalid 'start_time' at index ");
//       Serial.println(i);
//       continue;
//     }

//     const char* rawStart = (const char*)entry["start_time"];
//     if (rawStart == nullptr) {
//       Serial.print("Null start_time pointer at index ");
//       Serial.println(i);
//       continue;
//     }

//     String startStr = String(rawStart);
//     if (startStr.length() < 19) {
//       Serial.print("start_time too short at index ");
//       Serial.println(i);
//       continue;
//     }

//     // Parse safely
//     int year = startStr.substring(0, 4).toInt();
//     int month = startStr.substring(5, 7).toInt();
//     int day = startStr.substring(8, 10).toInt();
//     int hour = startStr.substring(11, 13).toInt();
//     int minute = startStr.substring(14, 16).toInt();
//     int second = startStr.substring(17, 19).toInt();

//     if (year < 2020 || month < 1 || month > 12 || day < 1 || day > 31 || hour > 23 || minute > 59 || second > 59) {
//       Serial.print("Invalid timestamp at index ");
//       Serial.println(i);
//       continue;
//     }

//     struct tm tmStart = {};
//     tmStart.tm_year = year - 1900;
//     tmStart.tm_mon = month - 1;
//     tmStart.tm_mday = day;
//     tmStart.tm_hour = hour;
//     tmStart.tm_min = minute;
//     tmStart.tm_sec = second;

//     time_t eventTime = mktime(&tmStart);

//     if (eventTime == -1) {
//       Serial.print("mktime failed at index ");
//       Serial.println(i);
//       continue;
//     }

//     if (eventTime > now && (soonest == 0 || eventTime < soonest)) {
//       soonest = eventTime;
//       nextEvent = entry;
//     }
//   }

//   if (soonest != 0 && JSON.typeof(nextEvent) == "object") {
//     Serial.println("Next upcoming booking:");
//     Serial.print("Title: ");
//     Serial.println((const char*)nextEvent["title"]);
//     Serial.print("Start Time: ");
//     Serial.println((const char*)nextEvent["start_time"]);
//     Serial.print("Type: ");
//     Serial.println((const char*)nextEvent["type"]);
//   } else {
//     Serial.println("No upcoming bookings found for today.");
//   }
// }