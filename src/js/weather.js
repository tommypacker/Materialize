Pebble.addEventListener('ready', 
	function(e){
		console.log('PebbleKit JS ready!');
		getWeather();
	}
);

Pebble.addEventListener('appmessage',
	function(e){
		console.log('AppMessage received!');
		getWeather();
	}
);

var xhrRequest = function(url, type, callback){
	var xhr = new XMLHttpRequest();
	xhr.onload = function(){
		callback(this.responseText);
	};
	xhr.open(type, url);
	xhr.send();
}

function locationSuccess(pos){
	var myAPIKey = "6d75d60fe049f78a10ec9feadcb151a9";
	var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
 		pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

 	xhrRequest(url, 'GET',
 		function(responseText){
 			var json = JSON.parse(responseText);
 			var temperature = Math.round(json.main.temp -273.15);
 			console.log('Remperature is ' + temperature);
 			var conditions = json.weather[0].main;
 			console.log('Conditions are ' + conditions);
 		}
 	);

}

function locationError(){
	console.log('Error requesting locaiton!');
}

function getWeather(){
	navigator.geolocation.getCurrentPosition(
		locationSuccess, 
		locationError,
		{timeout: 15000, maximumAge: 60000}
	);
}

var dictionary = {
	'KEY_TEMPERATURE': temperature,
	'KEY_CONDITIONS': conditions
}

Pebble.sendAppMessage(dictionary,
	function(e){
		console.log('Weather info sent to Pebble successfully!');
	},
	function(e){
		console.log('Error sending weather info to Pebble!');
	}
);


