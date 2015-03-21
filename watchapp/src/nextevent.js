var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getNextEvent() {
  // Replace with your server api url here
  var url = "http://YOUR SERVER API URL HERE";
  
  // Send request to api
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with NextEvent info
      var json = JSON.parse(responseText);
      var eventstart = json.start;      
      var eventend = json.end;      
      var eventlocation = json.location;      
      var eventtimeto = json.timeto;
      var eventtitle = json.title;
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_EVENTSTART": eventstart,
        "KEY_EVENTEND": eventend,
        "KEY_EVENTLOCATION": eventlocation,
        "KEY_EVENTTIMETO": eventtimeto,
        "KEY_EVENTTITLE": eventtitle
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("NextEvent info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending NextEvent info to Pebble!");
        }
      );
    }   
  );  
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e) {
    console.log("PebbleKit JS ready!");
    getNextEvent();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getNextEvent();
  }
);
