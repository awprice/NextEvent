import bottle
from wsgilog import WsgiLog
import string
import random
from icalendar import Calendar, Event
import requests
import datetime
import time
import json
from pytz import timezone

app = bottle.app()

@app.route('/nextevent/', method='GET')
def nextEvent():

        currenttime = int(time.time())

        icsfile = requests.get("YOUR GOOGLE CALENDAR FILE").text
        cal = Calendar.from_ical(icsfile)

        format = "%H:%M"

        events = {}
        timestamps = []

        for event in cal.walk('vevent'):

                timestart = event['DTSTART'].dt.astimezone(timezone('Australia/Sydney'))
                timeend = event['DTEND'].dt.astimezone(timezone('Australia/Sydney'))
                timestartunix = timestart.strftime("%s")

                eventdetails = {"title": str(event['SUMMARY'])[:13], "location": str(event['LOCATION']).strip(), "start": cleanTime(timestart.strftime(format)), "end": cleanTime(timeend.strftime(format)), "timeto": getTimeTo(timestartunix)}

                events[timestartunix] = eventdetails
                timestamps.append(int(timestartunix))

        timestamps.sort()

        nextfound = False

        for timestamp in timestamps:
                if currenttime < timestamp and nextfound == False:
                        nextfound = True
                        next = events[str(timestamp)]

        bottle.response.content_type = 'application/json'
        return json.dumps(next)

def getTimeTo(unix, default="now"):

        offset = datetime.datetime.now(timezone('Australia/Sydney')).strftime('%z')
        offset = offset.strip("00")
        offset = offset.strip("+")
        offset = int(offset) * 60 * 60

        workingdatetime = datetime.datetime.fromtimestamp(int(unix) - offset).strftime('%Y-%m-%d %H:%M:%S')

        now = datetime.datetime.utcnow()
        diff = datetime.datetime.strptime(workingdatetime, "%Y-%m-%d %H:%M:%S") - now

        periods = (
                (diff.days / 365, "year", "years"),
                (diff.days / 30, "month", "months"),
                (diff.days / 7, "week", "weeks"),
                (diff.days, "day", "days"),
                (diff.seconds / 3600, "hour", "hours"),
                (diff.seconds / 60, "minute", "minutes"),
                (diff.seconds, "second", "seconds"),
        )

        for period, singular, plural in periods:
                if period:
                        period += 1
                        return "in %d %s" % (period, singular if period == 1 else plural)

        return default

def cleanTime(time):

        splittime = time.split(":")

        if splittime[0][0] == "0":
                splittime[0] = splittime[0][1]

        if splittime[1] == "00":
                splittime[1] = ""
        else:
                splittime[1] = ":" + splittime[1]

        return splittime[0] + splittime[1]

@app.hook('before_request')
def before_request():
        global timebefore
        timebefore = datetime.datetime.now()

@app.hook('after_request')
def after_request():
        timeafter = datetime.datetime.now()
        difference = timeafter - timebefore
        print bottle.request.remote_addr + " - " + bottle.response.status_line + " - " + str(round(float(difference.microseconds) / 1000, 2)) + "ms - " + bottle.request.method + " - " + bottle.request.path

app = WsgiLog(app, tostream=True)
bottle.run(app=app, server='cherrypy', host='0.0.0.0', port=8080, reloader=True)
