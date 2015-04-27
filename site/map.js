/****************************************************************************************
 * map.js                                                                               *
 *                                                                                      *
 * Nolan Hawkins                                                                        *
 *                                                                                      *
 * For the display of crime and restauraunt data from the city of Boston.               *
 *                                                                                      *
 ****************************************************************************************/

/****************************************************************************************
 * Global variables: we'll have a few of these                                          *
 ****************************************************************************************/

/* These are the IDs of the Google Fusion Tables;
 * 
 * restaurauntTable is a table that stores
 *      Location, Name, establishment date, address, description, CrimeCost,
 *      and a list of crimes commited within 100 meters of the establishment
 * for the almost 3000 licensed restauraunts in the city of Boston
 * 
 * crimeTable is a table that stores
 *      Location, date, type [an integer that corresponds to a incident report], 
 *      and danger [a heuristic I calculated based on the type and the weapons involved]
 */
var restaurauntTable = '1b9aRnr4iFSUJwi6_MsYcHsvZLLlSm8oJuaP8w0S_';
var crimeTable       = '1mvUB-SB0yEct7hbGAVtsFogvZwSox4WBqvnSxFEr';

// some elements to save and prevent repeated calls to the longwinded document.getElementById
var searchBar, rankings, rankingSort, mapCanvas;

// This will be a 2 dimensional array, a list of arrays that represent restauraunts
// returned by a query into the restauraunt table
var restauraunts;

// This will store all the markers when the map does not display every restauraunt in a FusionTablesLayer
// It will be constructed from the array restauraunts
var restaurauntMarkers = [];

/* These are bits of the google maps API. map represents the map, crimeLayer the heatmap of crime, 
 * restaurauntLayer the display of all of the restauraunts, and infoWindow the window that pops up
 * with information on a restauraunt that has been clicked
 */
var map, crimeLayer, restaurauntLayer, infoWindow;




/****************************************************************************************
 * Initialization                                                                       *
 ****************************************************************************************/

google.maps.event.addDomListener(window, 'load', initialize);

function initialize() {
    var mapOptions = {
        center: { lat: 42.32203502633908, lng: -71.11280167236328 },
        zoom: 12,
        // Make the styles a bit more suited to our purposes: skinny roads and removed 
        // places of interest except for parks
        styles:[{ "featureType": "road",
                    "stylers": [{"weight": 0.9}]},
                { "featureType": "poi",
                    "stylers": [{"visibility": "off"}]},
                { "featureType": "poi.park",
                    "stylers": [{"visibility": "on"}]}
                ],
        // Move the default controls around to accomodate the search bar and results window
        mapTypeControl: false,
        panControl: true,
        panControlOptions: {
            position: google.maps.ControlPosition.RIGHT_TOP
        },
        zoomControl: true,
        zoomControlOptions: {
            style: google.maps.ZoomControlStyle.LARGE,
            position: google.maps.ControlPosition.RIGHT_TOP
        },
        scaleControl: true,
        streetViewControl: false
    };

    // Start the map!!
    map = new google.maps.Map(document.getElementById('map-canvas'),
                                mapOptions);
    
    // Set up the heatmap of crimes from crimeTable
    crimeLayer = new google.maps.FusionTablesLayer({
        query: {
            select: 'Location',
            from: crimeTable
        },
        heatmap: {
            enabled: true
        }

    });
    crimeLayer.setMap(map);
    
    // Set up the map of all the restauraunts from restaurauntTable
    restaurauntLayer = new google.maps.FusionTablesLayer({
        query: {
            select: 'Location',
            from: restaurauntTable
        },
        // Ideally each marker would be colored according to the safety, but FusionTable rendering
        // doesn't allow this, and rendering 3000 restauraunts clientside is too heavy 
        styles:[{markerOptions:{iconName:'measle_white'}}],
        suppressInfoWindows:true
    });
    
    // infoWindow will contain information on any restauraunt that is clicked
    infoWindow = new google.maps.InfoWindow({
        content: ''
    });
    
    // When a restauraunt is clicked, open an infoWindow with information about the restauraunt
    google.maps.event.addListener(restaurauntLayer, 'click', function(event) {
        var info = event.row;
        console.log(event);
        generateInfoWindowHTML({Location:info.Location.value,
                                Name:info.Name.value,
                                Date:info.Date.value,
                                Address:info.Address.value,
                                Description:info.Description.value,
                                CrimeCost:info.CrimeCost.value,
                                Crimes:info.Crimes.value
                               });
        infoWindow.setPosition(event.latLng);
        infoWindow.open(map);
    }); 
    restaurauntLayer.setMap(map);
    
    
    // Initialize the elements so as not to call document.getElementById so many times
    searchBar = document.getElementById("searchBar");
    rankings  = document.getElementById("rankings");
    restaurauntRankings = document.getElementById("restaurauntRankings");
    rankingSort = document.getElementById("rankingSort");
    mapCanvas   = document.getElementById("map-canvas");
    modalInfo   = document.getElementById("modalInfo");
    
    // set up modal info window
    document.getElementById('modalInfoClose').onclick = function(e){
      setClass(modalInfo, "minimized", true);
    };
    document.getElementById('modalInfoOpen').onclick = function(e){
      setClass(modalInfo, "minimized", false);
    };
    
    // if enter is hit, search for restauraunts
    searchBar.onkeypress = function(e){
        e = e || window.event;
        // a keycode of 13 means Enter was pressed
        if (e.keyCode == 13)
            selectRestauraunts();
    }
}




/****************************************************************************************
 * Searching                                                                            *
 *                                                                                      *
 * This section controls the machanics of searching for a restauraunt or a bunch of     *
 * restauraunts, displaying both their position on the map and their safety through     *
 * coloring and rank among the search results                                           *
 ****************************************************************************************/

// This function is called by searching, and changes the map and rankings according to the search
// by querying the restaurauntTable
function selectRestauraunts(){
    var value = searchBar.value;
    
    // deletes all the restauraunt markers, as they are rendered irrelevent by the search
    for(var i=0;i<restaurauntMarkers.length;i++){
        restaurauntMarkers[i].setMap(null);
    }
    restaurauntMarkers=[];
    
    if(value.length > 0){ 
        var url = 'https://www.googleapis.com/fusiontables/v2/query?key='+apiKey;
        url += '&sql=SELECT Location, Name, Address, CrimeCost FROM ';
        url += restaurauntTable;
        // The search is simply for containing strings. A different kind of search would be interesting
        // but for now this works well for chains and things like 
        // "restauraunts that include the word pizza"
        url += ' WHERE Name CONTAINS IGNORING CASE \'' + value.replace('\'','\\\'') + '\'';
        url += ' ORDER BY CrimeCost ASC';
        var x = new XMLHttpRequest();
        x.open('POST', url, true);
        x.onreadystatechange = function(){
            if (x.readyState == 4 && x.status == 200) {
                // when a response is recieved, update restauraunts.
                updateRestauraunts(JSON.parse(x.responseText));
            }
        };
        x.send();
    }else{
        // if the search bar is empty, revert to displaying every restauraunt
        restaurauntLayer.setMap(map);
        setClass(rankings, 'minimized', true);
	setClass(mapCanvas, 'minimized', false);
	//setClass

    }
}

// This function is called with the JSON of the response to the query into restaurauntTable by
// selectRestauraunts, and updates the map and rankings with the search results
function updateRestauraunts(data){
    // hides the rest of the restauraunts
    restaurauntLayer.setMap(null);
    
    restauraunts = data.rows;
    
    // Sets up the restauraunt markers
    for(var i=0;i<restauraunts.length;i++){
        restaurauntMarkers[i] = new google.maps.Marker({
            position: parseLatLng(restauraunts[i][0]),
            map: map,
            title:restauraunts[i][1],
            icon: pinSymbol(generateColorFromSafety(restauraunts[i][3]))
        });
        restaurauntMarkers[i].setMap(map);
        
        // Use a closure to add a unique event listener for each marker
        (function(i){
            google.maps.event.addListener(restaurauntMarkers[i], 'click', function() {
                    var info = restauraunts[i];
                    generateInfoWindowHTML({Location:info[0],
                                            Name:info[1],               
                                            Address:info[2],
                                            CrimeCost:info[3]});
                    infoWindow.open(map, restaurauntMarkers[i]);
            });
        })(i);

    }
    setRankings();
}

// Fills up the rankings element with restaraunts from the search
function setRankings(){
    //If there are less then 10 matching restauraunts, hide the sorting elements
    setClass(rankingSort, 'hidden', restauraunts.length < 10);
    setAscending(true);
    setClass(rankings, 'minimized', false);
    setClass(mapCanvas, 'minimized', true);
}  

// Of restauraunts returned from a search, this
// displays either the top 10 safest restauraunts if ascending is true,
// or the top 10 most dangerous restauraunts if ascending is false.
function setAscending(ascending){
    // removes all children from the element restaurauntRankings
    while(restaurauntRankings.firstChild)
        restaurauntRankings.removeChild(restaurauntRankings.firstChild);
        
    if(ascending){
        for(var i=0;i<10 && i <restauraunts.length; i++){
            restaurauntRankings.appendChild(generateRestaurauntRankInfo(i));
        }
    }else{
        for(var i=restauraunts.length-1;i>=0 && i >restauraunts.length - 11; i--){
            restaurauntRankings.appendChild(generateRestaurauntRankInfo(i));
        }
    }
}

// This function adds a restauraunt stored at index of index in the array restauraunts
// to the rankings with a name, address, and safety ranking of the restauraunt
function generateRestaurauntRankInfo(index){
    var node = document.createElement("DIV");
    node.className = "restaurauntInfoBlock";
    var info = restauraunts[index];
    node.innerHTML = '<span class="safetyInfo"></span>' +
                     '<span class="restaurauntName">'+info[1]+'</span><br />' +
                     '<span class="restaurauntAddress">'+info[2]+'</span>';
    node.firstChild.style.backgroundColor=generateColorFromSafety(info[3]);
    // When the ranking is clicked, open the infoWindow on the map
    node.onclick=function(e){
        generateInfoWindowHTML({Location:info[0], Name:info[1], Address:info[2], CrimeCost:info[3]});
        infoWindow.open(map, restaurauntMarkers[index]);
    };
    
    // When the ranking is hovered over, animate the corresponding marked so as to notice it on the map
    node.onmouseover = function(){
        restaurauntMarkers[index].setAnimation(google.maps.Animation.BOUNCE);
    };
    node.onmouseout = function(){
        restaurauntMarkers[index].setAnimation(null);
    };
    return node;
}
 

 
 
/****************************************************************************************
 * Information Display                                                                  *
 *                                                                                      *
 * This section deals with displaying the infoWindow element for restauraunts when they *
 * are clicked. Not all of the information is stored locally, so some has to be queried *
 * some of the time                                                                     *
 ****************************************************************************************/

// Get all information on a restauraunt, given it's (hopefully unique) combination 
// of name and address
function loadMoreInfoWindowHTML(name, address){
    var url = 'https://www.googleapis.com/fusiontables/v2/query?key='+apiKey;
    url += '&sql=';
    var select = 'SELECT * FROM '+ restaurauntTable;
    select    += ' WHERE Name = \''+name.replace('\'','\\\'') +'\'';
    select    += ' AND Address = \'' + address+'\''
    select    += ' LIMIT 1';
    url += encodeURIComponent(select);
    console.log(url);
    var x = new XMLHttpRequest();
    x.open('POST', url, true);
    x.onreadystatechange = function(){
        if (x.readyState == 4 && x.status == 200) {
            // When the response is recieved, update the infoWindow
            var info = JSON.parse(x.responseText).rows[0];
            generateInfoWindowHTML({Location:info[0],
                                    Name:info[1],
                                    Date:info[2],
                                    Address:info[3],
                                    Description:info[4],
                                    CrimeCost:info[5],
                                    Crimes:info[6]
                                   });
        }
    };
    x.send();
}

/* This function fills the infoWindow with information on the restauraunt clicked. The argument
 * restauraunt is either an object with only Name, Address, Location and CrimeCost attributes,
 * with the other attributes needing to be loaded, or it also has Date, Description, and Crimes,
 * which is the rest of the data stored in the restaurauntTable
 */
function generateInfoWindowHTML(restauraunt){
    var col = generateColorFromSafety(restauraunt.CrimeCost);
    if(!('Date' in restauraunt)){
        infoWindow.setContent("<div><h1>"+restauraunt.Name +"</h1>"+
            '<div class="address">'+restauraunt.Address+'</div>'+
            '<div class="safetyRating">'+
            '<span class="safetyInfo" style="background-color:'+col+'"></span>'+
            'Danger Rating: '+restauraunt.CrimeCost+'</div>');
        loadMoreInfoWindowHTML(restauraunt.Name, restauraunt.Address);
    }else{
        console.log(restauraunt.Crimes);
        infoWindow.setContent("<div><h1>"+restauraunt.Name +"</h1>"+
            '<div><i>'+restauraunt.Description +', EST '+restauraunt.Date+'</i></div>'+
            '<div class="address">'+restauraunt.Address+'</div>'+
            '<div class="safetyRating">'+
            '<span class="safetyInfo" style="background-color:'+col+'"></span>'+
            'Danger Rating: '+restauraunt.CrimeCost+'</div><hr />'+
            processCrimes(restauraunt.CrimeCost, restauraunt.Crimes));
    }     
}

/* This returns a description of the crimes commited around a restauraunt given the last
 * two columns of the data for a restauraunt in restaurauntTable: CrimeCost (a score, with
 * 0 indicating no crimes), and Crimes, a list of crimes stored in the format of 
 *      "|date~crimeType~weaponType" repeated for each crime
 * crimeType is an integer that I don't actually care about at the moment, it represents
 * one of 29 basic "incident types" described in the crime report, but while some
 * of them are clear, others are police jargon. Consequetually, I ignored it.
 */
function processCrimes(danger, crimeList){
    if(danger == 0)
        return '<div> There are 0 crimes reported within 100 meters of this establishment</div>';
    
    // given the format, this will return a list with length of 1 greater than the length
    // of the array of crimes
    crimeList = crimeList.split('|');
        
    // Find the date of the oldest crime listed:
    var oldestYear = crimeList[1].split('~')[0].split('/')[2];
    var crimeCount = crimeList.length-1;
    var gunCrimes, knifeCrimes, shootings, weaponCrimes;
    gunCrimes = knifeCrimes = shootings = weaponCrimes = 0;
    for(var i = 0;i < crimeList.length;i++){
        var weaponFlags = parseInt(crimeList[i].split('~')[2]);
        if(weaponFlags&4)
            shootings++;
        if(weaponFlags == 0)
            continue;
        weaponCrimes++;
        switch(weaponFlags&3){
            case 1:
                // weaponType is other
                break;
            case 2:
                knifeCrimes++;
                break;
            case 3:
                gunCrimes++;
                break;
        }
    }
    var response = '<div>There have been '+crimeCount+
                   ' crimes reported within 100 meters of this establishment since '+oldestYear;
    if(weaponCrimes == 0){
        return response+', none of which involved weapons.</div>';
    }else{
        response += '. In '+weaponCrimes+' of those, the suspect was armed'
        if(shootings > 0)
            response += ', and in '+shootings+' of those, there was shooting';
        response+='. ';
        if(gunCrimes > 0)
            response += 'In '+gunCrimes+', the suspect had a firearm. ';
        if(knifeCrimes > 0)
            response += 'In '+knifeCrimes+', the suspect had a knife.';
        return response+'</div>';
    }
}




/****************************************************************************************
 * Various tools used in other functions                                                *
 ****************************************************************************************/

// Given a latitude and longitude in the form of "###, ###", this returns a google maps latLng object
function parseLatLng(latLngString){
    var commaPos = latLngString.indexOf(',');
    return new google.maps.LatLng(parseFloat(latLngString.substr(0,commaPos)),
                                  parseFloat(latLngString.substr(commaPos + 2)));
}

// returns a circular symbol to be used as a marker. This was the best way to have customized
// marker colors on the map
function pinSymbol(color) {
    return {
                path: 'M 0, 0 m -5, 0 a 5,5 0 1,0 10,0 a 5,5 0 1,0 -10,0',
                fillColor: color,
                fillOpacity: 1,
                strokeColor: '#000',
                strokeWeight: 1,
                scale: 1,
            };
}

// Converts from HSV to RGB for coloring the safety indicators with a nice gradient
// This particular code was copied almost entirely from the StackOverflow thread at 
// http://stackoverflow.com/questions/17242144/javascript-convert-hsb-hsv-color-to-rgb-accurately
function HSVtoRGB(h, s, v) {
    var r, g, b, i, f, p, q, t;
    i = Math.floor(h * 6);
    f = h * 6 - i;
    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - (1 - f) * s);
    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
    return 'rgb('+ Math.floor(r * 255).toString() + ', '
                    + Math.floor(g * 255).toString() + ', '
                    + Math.floor(b * 255).toString() + ')';
}

// This takes in a CrimeCost of a restauraunt, typically between 0 (being the safest) and 100 
// (being dangerous, although the CrimeCost can exceed 100 by a large amount), and generates a
// CSS color in the form of 'rgb(r,g,b)'
function generateColorFromSafety(safety){
    if(safety > 100){
        safety = 100;
    }
    safety = Math.sqrt(safety/100);
    return HSVtoRGB((1-safety)/3.0, 1, 1);
}


// a quick tool for modifying the class of an element,
// This either ensures elem has the class className if toggle is true
// Or ensures that elem doesn't have the class className if toggle is false
function setClass(elem, className, toggle){
    if (typeof elem.className == 'undefined'){
        elem.className="";
    }
    var index = elem.className.indexOf(className)
    if(index == -1 && toggle)
        elem.className += " "+className;
    else if(index > -1 && !toggle)
        elem.className = elem.className.substr(0, index) + 
                         elem.className.substr(index+className.length+1);
} 
