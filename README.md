NextEvent
===========

This is a Watchface for the Pebble Watch. I designed and made it as a way of clearly seeing the time on the watch, but also being able to see when my next class/lecture/appointment was for University.

It requires a server to host the server application component of the application and the watch itself for the watch application. I elected to have a server involved with the process as I need somewhere to download the calendar (as in ics file) from Google Calendar, calculate what the next event was, then return a json object with all of the necessary data. It would mean I could save bandwidth. My phone which would be connected to the watch wouldn't have to download the ics file and then have the watch itself work out the next event all on the fly.

##Installation##

This watchface app + server combo is not the simplest in the world, but I didn't really design this to be distributed.

### Install the serverapp ###

1. Copy the contents of the serverapp directory onto your server. 
2. Install the required packages in the requirements.txt.
3. Replace the url of the Google Calendar file with your own.
4. Change the timezones from "Australia/Sydney" to your own if needed.
5. Start the server by running the python file.
6. Check if it is working at the correct location

### Install the watchapp ###

1. Copy the contents of the watchapp over to the cloudpebble.net dev environment
2. Test and make sure it is working by compiling on both the test watch and your own watch

## JSON Object ##

Here is what the JSON object that is sent to the watchapp should look like: 

```JSON
}
  "start":"9",
  "end":"10:30",
  "location":"Building 1",
  "timeto":"in 22 hours",
  "title":"Piano Recital"
}
```

## Some things to note ##

 - The title for the event is trimmed to 13 characters to fit and look nice on the tiny Pebble watch screen
 - The Google Calendar file is downloaded every time a request is made for the JSON object. Not very good to download it every time, but it works for now...
