'use strict'

// C library API
const ffi = require('ffi-napi');

let sharedLib = ffi.Library('./parser/bin/libgpxparser', {
  'createGPXtoJSON': [ 'string', ['string' ] ],		//return type first, argument list second
  'createGPXtoTracksJSON' : [ 'string', ['string' ] ],
  'createGPXtoRoutesJSON' : [ 'string', ['string' ] ],
  'routesOtherDataToJSON' : [ 'string', ['string' ] ],
  'tracksOtherDataToJSON' : [ 'string', ['string' ] ],
  'rtePtToJSON' : [ 'string', ['string' ] ],

});

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

function uploadedFilesToGPX() {
  const files = fs.readdirSync(path.join(__dirname+'/uploads'));
  var allGpx = [];
  for (const file of files) {
    let name = 'uploads/'+ file;
    if (name.endsWith(".gpx")) {
    let gpxJSON = sharedLib.createGPXtoJSON(name);
    let gpx = JSON.parse(gpxJSON);
    allGpx.push(gpx);
  } else {
    console.log('not a valid gpx file');
  }
  }

  var gpxesJSON = JSON.stringify(allGpx);
  //var gpxess = JSON.parse(gpxesJSON);
  //console.log(gpxesJSON);
  return gpxesJSON;
}

function getFileNames(){
  const files = fs.readdirSync(path.join(__dirname+'/uploads'));
  var allNames = [];
  for (const file of files) {
    let name = file;
    if (name.endsWith(".gpx")) {
     allNames.push(name);
  }
  }
  var namesJSON = JSON.stringify(allNames);
  return namesJSON;
}

function getRoutesJson(param) {

  var allGpx = [];
  
    let name = 'uploads/'+ param;
    if (name.endsWith(".gpx")) {
    let gpxJSON = sharedLib.createGPXtoRoutesJSON(name);
    let gpx = JSON.parse(gpxJSON);
    allGpx.push(gpx);
  } else {
    console.log("not a gpx file");
  }
  
  var routesJSON = JSON.stringify(allGpx);
  return routesJSON;
}
function getTracksJson(param) {

  var allGpx = [];
  
    let name = 'uploads/'+ param;
    if (name.endsWith(".gpx")) {
    let gpxJSON = sharedLib.createGPXtoTracksJSON(name);
    let gpx = JSON.parse(gpxJSON);
    allGpx.push(gpx);
  } else {
    console.log("not a gpx file");
  }
  
  var tracksJSON = JSON.stringify(allGpx);
  return tracksJSON;
}
function getRoutesDataJson(param) {

  var allGpx = [];
  
    let name = 'uploads/'+ param;
    if (name.endsWith(".gpx")) {
    let gpxJSON = sharedLib.routesOtherDataToJSON(name);
    let gpx = JSON.parse(gpxJSON);
    allGpx.push(gpx);
  } else {
    console.log("not a gpx file");
  }
  
  var routesJSON = JSON.stringify(allGpx);
  return routesJSON;
}
function getTracksDataJson(param) {

  var allGpx = [];
  
    let name = 'uploads/'+ param;
    if (name.endsWith(".gpx")) {
    let gpxJSON = sharedLib.tracksOtherDataToJSON(name);
    let gpx = JSON.parse(gpxJSON);
    allGpx.push(gpx);
  } else {
    console.log("not a gpx file");
  }
  
  var tracksJSON = JSON.stringify(allGpx);
  return tracksJSON;
}

async function connectAndCreateDB(dbConf) {
  const mysql = require('mysql2/promise');
    
  let connection;
  try{
    // create the connection
    connection = await mysql.createConnection(dbConf)

    
     await connection.execute("CREATE TABLE IF NOT EXISTS FILE (gpx_id INT AUTO_INCREMENT PRIMARY KEY,  file_name VARCHAR(60) NOT NULL,  ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) not null )");
     await connection.execute("CREATE TABLE IF NOT EXISTS ROUTE (route_id INT AUTO_INCREMENT PRIMARY KEY,  route_name VARCHAR(256) ,  route_len FLOAT(15,7) NOT NULL, gpx_id INT not null, FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE ");
     await connection.execute("CREATE TABLE IF NOT EXISTS POINT (point_id INT AUTO_INCREMENT PRIMARY KEY,  point_index INT NOT NULL ,  latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256), route_id INT not null, FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE ");
   // await connection.execute(insRec);


}catch(e){
    console.log("Query error: "+e);
}finally {
    if (connection && connection.end) connection.end();
}
}

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
  res.send(
    {
      somethingElse: retStr
    }
  );
});

app.get('/gpxInfos', function(req , res){
  let retStr = uploadedFilesToGPX();
  let retStr2 = getFileNames();
  res.send(
    {
      gpxJSON: retStr,
      namesJSON: retStr2
    }
  );
});

app.get('/gpxView', function(req , res){
  
  let retStr = getRoutesJson(req.query.name);
  let retStr2 = getTracksJson(req.query.name);
 // let routeData = getRoutesDataJson(req.query.name);
 // let trackData = getTracksDataJson(req.query.name);
 
  res.send(
    {
      routesJSON: retStr,
      tracksJSON: retStr2,
   //   routeDataJSON: routeData,
    //  trackDataJSON: trackData
    
    }
  );
});
app.get('/createNewGpx', function(req , res){
  
 // let retStr = createNewGpx(req.query.name);
  
 
  res.send(
    {
    
    }
  );
});

app.get('/connectDB', async function(req, res, next){
  // connectAndCreateDB(req.query.dbConf);
  const mysql = require('mysql2/promise');
  var fileCount = 0;
  var routeCount = 0;
  var pointCount = 0;
    let connection;
    try{
      // create the connection
      connection = await mysql.createConnection(req.query.dbConf)

      
      await connection.execute('CREATE TABLE IF NOT EXISTS FILE (gpx_id INT AUTO_INCREMENT PRIMARY KEY,  file_name VARCHAR(60) NOT NULL,  ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) not null )');
      await connection.execute('CREATE TABLE IF NOT EXISTS ROUTE (route_id INT AUTO_INCREMENT PRIMARY KEY,  route_name VARCHAR(256) ,  route_len FLOAT(15,7) NOT NULL, gpx_id INT not null, FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE) ');
      await connection.execute('CREATE TABLE IF NOT EXISTS POINT (point_id INT AUTO_INCREMENT PRIMARY KEY,  point_index INT NOT NULL ,  latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256), route_id INT not null, FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE )');

      const [rows1, fields1] = await connection.execute('SELECT * from `FILE');
      for (let row of rows1){
       fileCount = fileCount + 1;
      }

      const [rows2, fields2] = await connection.execute('SELECT * from `ROUTE');
      for (let row of rows2){
        routeCount = routeCount + 1;
      }

      const [rows3, fields3] = await connection.execute('SELECT * from `POINT');
      for (let row of rows3){
        pointCount = pointCount + 1;
      }


  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }
   res.send(
    {
      fc: fileCount,
      rc: routeCount,
      pc: pointCount,
    });
  });

app.get('/storeDB', async function(req, res, next){
  
     const mysql = require('mysql2/promise');
      var fileCount = 0;
      var routeCount = 0;
      var pointCount = 0;
      let connection;
      try{
        // create the connection
        connection = await mysql.createConnection(req.query.dbConf)
        const files = fs.readdirSync(path.join(__dirname+'/uploads'));
        var fileStr = "";
        var routeStr = "";
        var pointStr = "";
        var i = 1;
        var j = 1;
        var z = 1;
     
        for (const file of files) {
          fileStr = "INSERT INTO  FILE (gpx_id, file_name, ver, creator) VALUES(";
          
          let name = 'uploads/'+ file;
          if (name.endsWith(".gpx")) {
            fileStr = fileStr + i +",'" + file + "','";
          
            let gpxJSON = sharedLib.createGPXtoJSON(name);
            let gpx = JSON.parse(gpxJSON);
            fileStr = fileStr + gpx.version + "','" + gpx.creator + "') ON DUPLICATE KEY UPDATE file_name = file_name";
            console.log(fileStr);
            
            await connection.execute(fileStr);
            let rteJSON = sharedLib.createGPXtoRoutesJSON(name);
            let route = JSON.parse(rteJSON);
            // get gpx id
            
            for (let rte of route){
              routeStr = "INSERT INTO  ROUTE (route_id, route_name, route_len, gpx_id) VALUES(";
              routeStr = routeStr + j + ",'" +rte.name + "','" + rte.len +  "','" + i + "') ON DUPLICATE KEY UPDATE route_len = VALUES(route_len)";
              console.log(routeStr);
              await connection.execute(routeStr);
              j++
            }

            let pteJSON = sharedLib.rtePtToJSON(name);
            let points = JSON.parse(pteJSON);
            for (let pt of points) {
              let routeId = pt.route_id + i -1;
              pointStr = "INSERT INTO  POINT (point_id, point_index, latitude, longitude, point_name, route_id) VALUES(";
              pointStr = pointStr + z+ ",'"+ pt.point_index + "','" + pt.latitude+ "','" + pt.longitude + "','"+ pt.name + "','"+ routeId + "') ON DUPLICATE KEY UPDATE point_index = point_index";
              console.log(pointStr);
              await connection.execute(pointStr);

              z++;
            }

         i++;
        }
        }
        // get data base status
        const [rows1, fields1] = await connection.execute('SELECT * from `FILE');
        for (let row of rows1){
           fileCount = fileCount + 1;
        }

        const [rows2, fields2] = await connection.execute('SELECT * from `ROUTE');
        for (let row of rows2){
           routeCount = routeCount + 1;
        }

        const [rows3, fields3] = await connection.execute('SELECT * from `POINT');
        for (let row of rows3){
           pointCount = pointCount + 1;
        }
       

  
  
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) connection.end();
    }
     res.send(
      {
        fc: fileCount,
        rc: routeCount,
        pc: pointCount,
      });
    });

app.get('/clearDB', async function(req, res, next){

      const mysql = require('mysql2/promise');
        
        var fileCount = 0;
        var routeCount = 0;
        var pointCount = 0;
        let connection;
        try{
          // create the connection
          connection = await mysql.createConnection(req.query.dbConf)
    
          
          await connection.execute('DELETE  FROM POINT');
          await connection.execute('DELETE  FROM ROUTE');
          await connection.execute('DELETE  FROM FILE');


          const [rows1, fields1] = await connection.execute('SELECT * FROM `FILE');
          for (let row of rows1){
           fileCount = fileCount + 1;
          }
    
          const [rows2, fields2] = await connection.execute('SELECT * FROM `ROUTE');
          for (let row of rows2){
            routeCount = routeCount + 1;
          }
    
          const [rows3, fields3] = await connection.execute('SELECT * FROM `POINT');
          for (let row of rows3){
            pointCount = pointCount + 1;
          }
    
    
      }catch(e){
          console.log("Query error: "+e);
      }finally {
          if (connection && connection.end) connection.end();
      }
       res.send(
        {
          fc: fileCount,
          rc: routeCount,
          pc: pointCount,
        });
      });
app.listen(portNum);
console.log('Running app at localhost: ' + portNum);