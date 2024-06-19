// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example

    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/gpxInfos',   //The server endpoint we are connecting to
        data: {
 
        },
        success: function (data) {
            
            information = JSON.parse(data.gpxJSON);
            links = JSON.parse(data.namesJSON);

            $("#tbody1").html("");
            $("GPXview").html("");
            var i = 0;
             for (let gpx of information) {

              $('#tbody1').append('<tr><td><a href="'+links[i]+'" download ">"'+links[i]+'"</a></td><td> "'+gpx.version+'"</td><td> "'+gpx.creator+'"</td><td> "'+gpx.numWaypoints+'"</td><td> "'+gpx.numRoutes+'"</td><td> "'+gpx.numTracks+'"</td></tr>');
              $("#GPXview").append('<option value= "'+links[i]+'">"'+links[i]+'"</option>');
              i = i +1;
            }
            

        },
        fail: function(error) {

            console.log(error); 
        }
    });
    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#create').submit(function(e){
        var newName = $('#entryBox').val();
       // e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/createNewGpx',   //The server endpoint we are connecting to
            
            data: {
                name: newName
            },
            success: function (data) {
                
       
            },
            fail: function(error) {
                console.log(error); 
            }
            
        });
    });

    $('#connectDB').submit(function(e){

        var db = {
            host     : 'dursley.socs.uoguelph.ca',
            user     : $('#user').val(),
            password : $('#pass').val(),
            database : $('#dbname').val()
        };

        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/connectDB',   //The server endpoint we are connecting to
            
            data: {
                dbConf: db
            },
            success: function (data) {
                
                alert('Successfully Connected to DB!\n Database has '+ data.fc+ "files, "+ data.rc + "routes and "+ data.pc+ " points");
            },
            fail: function(error) {

                alert('Failed to Connect- Please Try Again!');
                console.log(error); 
            }
            
        });
    });

    $('#storeFiles').submit(function(e){


        e.preventDefault();
        var db = {
            host     : 'dursley.socs.uoguelph.ca',
            user     : $('#user').val(),
            password : $('#pass').val(),
            database : $('#dbname').val()
        };
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/storeDB',   //The server endpoint we are connecting to
            
            data: {   
                dbConf: db     
            },
            success: function (data) {
                
                alert('Database has '+ data.fc+ "files, "+ data.rc + "routes and "+ data.pc+ " points");
            },
            fail: function(error) {

                alert('Failed to Connect- Please Try Again!');
                console.log(error); 
            }
            
        });
    });

    $('#clearFiles').submit(function(e){


        e.preventDefault();
        var db = {
            host     : 'dursley.socs.uoguelph.ca',
            user     : $('#user').val(),
            password : $('#pass').val(),
            database : $('#dbname').val()
        };
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/clearDB',   //The server endpoint we are connecting to
            
            data: {   
                dbConf: db     
            },
            success: function (data) {
                
                alert('Database has '+ data.fc+ "files, "+ data.rc + "routes and "+ data.pc+ " points");
            },
            fail: function(error) {

                alert('Failed to Connect- Please Try Again!');
                console.log(error); 
            }
            
        });
    });

    $('#GPXviewForm').submit(function(e){
         var selectedGPX = $("#GPXview option:selected").val();
         console.log(selectedGPX);
        e.preventDefault();
        
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/gpxView',   //The server endpoint we are connecting to
            
            data: {
                name: selectedGPX
            },
            success: function (data) {
                
                routes = JSON.parse(data.routesJSON);
                tracks = JSON.parse(data.tracksJSON);
               // routeData = JSON.parse(data.routeDataJSON);
               // trackData = JSON.parse(data.trackDataJSON);
                $("#tbody2").html("");
                var i = 1;
                 for (let rt of routes) {
                    for (let rte of rt) {
                    let id = 'rte'+ i;   
                    $('#tbody2').append('<tr><td>Route"'+i+'"</td><td> "'+rte.name+'"</td><td> "'+rte.numPoints+'"</td><td> "'+rte.len+'"</td><td> "'+rte.loop+'"</td><td> <input id ="'+id+'"type="button" value = "show" </td></tr>');
                     i = i +1;
                    }
                }
                i = 1;
                for (let rt of tracks) {
                    for (let rte of rt) {
                        let id = 'trk'+i;
                    $('#tbody2').append('<tr><td>Track"'+i+'"</td><td> "'+rte.name+'"</td><td> "'+rte.numPoints+'" </td><td> "'+rte.len+'"</td><td> "'+rte.loop+'"</td><td> "'+rte.len+'"</td><td> "'+rte.loop+'"</td><td> <input id ="'+id+'"type="button" value = "show" </td></tr>');
  
                    i = i +1;
                    }
                  }
                  i = 1;
                
                  
               //  $('#otherData').html('<br> routes other data(s): "'+routeData+'"');
                // $('#otherData').html('<br> tracks other data(s): "'+trackData+'"');
                          
                      
                  
                
                
                 
    
            },
            fail: function(error) {
                console.log("unable to get "+ selectedGPX + "data");
                console.log(error); 
            }
        });
    });
});