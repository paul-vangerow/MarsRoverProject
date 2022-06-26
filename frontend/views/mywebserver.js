
var express = require('express');
var server = express();
var bodyParser = require('body-parser');
var htmlParser = require('node-html-parser');
var fileSystem = require("fs");
server.use(bodyParser.json());
server.use(bodyParser.urlencoded({ extended: true }));
server.use(express.static('public'));
server.set('view engine', 'ejs');

// /home/ubuntu/index.html
server.get('/', function(req, res) {
    res.sendFile('/home/ubuntu/views/index.html');
});

server.get('/data', function(req, res) {
    var mysql = require('mysql2');
    var router = express.Router();

    var con = mysql.createConnection({
        host: "localhost",
        user: "PHPScript",
        password: "php_access",
        database: "map_list"
    });
    
    con.connect(function(err) {
        if (err) throw err;
        console.log("Connected!");
        con.query("SELECT id FROM maps ORDER BY assign_time DESC LIMIT 1", function (err, result) {
            if (err) throw err;
            
            var mapno = JSON.stringify(result[0].id);
            console.log(mapno);
            //JSON.stringify(result[0].x)
            var con = mysql.createConnection({
                host: "localhost",
                user: "PHPScript",
                password: "php_access",
                database: "map_" + mapno
            });
              
            con.connect(function(err) {
                if (err) throw err;
                console.log("Connected!");
                con.query("SELECT x,y,colour FROM alien_location", function (err, result) {
                    if (err) throw err;
                    console.log(result);
                    res.render('data.ejs', {yoma: result});
                    //JSON.stringify(result[0].x)
                });
            });
        });
    });

    
});

server.get('/control', function(req, res) {
    res.sendFile('/home/ubuntu/views/control.html');
});

server.get('/group', function(req, res) {
    res.sendFile('/home/ubuntu/views/group.html');
});

server.post('/forward', function(req, res) {

    var mysql = require('mysql2');
    const formData = req.body;
    var con = mysql.createConnection({
        host: "localhost",
        user: "PHPScript",
        password: "php_access",
        database: "map_list"
    });
    
    con.connect(function(err) {
        if (err) throw err;
        console.log("Connected!");
        con.query("SELECT id FROM maps ORDER BY assign_time DESC LIMIT 1", function (err, result) {
            if (err) throw err;
            var mapno = JSON.stringify(result[0].id);
            console.log(mapno);

            var con = mysql.createConnection({
                host: "localhost",
                user: "PHPScript",
                password: "php_access",
                database: "map_" + mapno
            });
        
            con.connect(function(err) {
                if (err) throw err;
                console.log("Connected!");
                var sql = "INSERT INTO instructions (instruction, val, executed, order_time, execute_time) VALUES (0, " + formData.num1 + ", false, null, null)";
                con.query(sql, function (err, result) {
                    if (err) throw err;
                    console.log("1 record inserted");
                });
            });
        });
    });
    res.sendFile('/home/ubuntu/views/control.html');
});

server.post('/rotate', function(req, res) {
    var mysql = require('mysql2');
    const formData = req.body;
    var con = mysql.createConnection({
        host: "localhost",
        user: "PHPScript",
        password: "php_access",
        database: "map_list"
    });
    
    con.connect(function(err) {
        if (err) throw err;
        console.log("Connected!");
        con.query("SELECT id FROM maps ORDER BY assign_time DESC LIMIT 1", function (err, result) {
            if (err) throw err;
            var mapno = JSON.stringify(result[0].id);
            console.log(mapno);

            var con = mysql.createConnection({
                host: "localhost",
                user: "PHPScript",
                password: "php_access",
                database: "map_" + mapno
            });
        
            con.connect(function(err) {
                if (err) throw err;
                console.log("Connected!");
                var sql = "INSERT INTO instructions (instruction, val, executed, order_time, execute_time) VALUES (1, " + formData.num2 + ", false, null, null)";
                con.query(sql, function (err, result) {
                    if (err) throw err;
                    console.log("1 record inserted");
                });
            });
        });
    });
    res.sendFile('/home/ubuntu/views/control.html');
});

server.post('/explore', function(req, res) {
    var mysql = require('mysql2');
    const formData = req.body;
    var con = mysql.createConnection({
        host: "localhost",
        user: "PHPScript",
        password: "php_access",
        database: "map_list"
    });
    
    con.connect(function(err) {
        if (err) throw err;
        console.log("Connected!");
        con.query("SELECT id FROM maps ORDER BY assign_time DESC LIMIT 1", function (err, result) {
            if (err) throw err;
            var mapno = JSON.stringify(result[0].id);
            console.log(mapno);

            var con = mysql.createConnection({
                host: "localhost",
                user: "PHPScript",
                password: "php_access",
                database: "map_" + mapno
            });
        
            con.connect(function(err) {
                if (err) throw err;
                console.log("Connected!");
                var sql = "INSERT INTO instructions (instruction, val, executed, order_time, execute_time) VALUES (2, null, false, null, null)";
                con.query(sql, function (err, result) {
                    if (err) throw err;
                    console.log("1 record inserted");
                });
            });
        });
    });
    res.sendFile('/home/ubuntu/views/control.html');
});

server.post('/stopexplore', function(req, res) {
    var mysql = require('mysql2');
    var con = mysql.createConnection({
        host: "localhost",
        user: "PHPScript",
        password: "php_access",
        database: "map_list"
    });
    
    con.connect(function(err) {
        if (err) throw err;
        console.log("Connected!");
        con.query("SELECT id FROM maps ORDER BY assign_time DESC LIMIT 1", function (err, result) {
            if (err) throw err;
            var mapno = JSON.stringify(result[0].id);
            console.log(mapno);

            var con = mysql.createConnection({
                host: "localhost",
                user: "PHPScript",
                password: "php_access",
                database: "map_" + mapno
            });
        
            con.connect(function(err) {
                if (err) throw err;
                console.log("Connected!");
                var sql = "INSERT INTO instructions (instruction, val, executed, order_time, execute_time) VALUES (3, null, false, null, null)";
                con.query(sql, function (err, result) {
                    if (err) throw err;
                    console.log("1 record inserted");
                });
            });
        });
    });
    res.sendFile('/home/ubuntu/views/control.html');
});

console.log('Server is running on port 3000'); 
server.listen(3000,'0.0.0.0');