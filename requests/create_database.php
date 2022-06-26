<?php

    include 'db_connect.php';

    // first connection to the database to declare a new databse
    $conn = Database::connect('map_list');
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    $time = date("Y-m-d H:i:s");

    $sql = "INSERT INTO maps(assign_time) VALUES('$time')";
    $conn->exec($sql);
    Database::disconnect();

    $map_id = getMap();
    echo $map_id;
    $dbname = "map_".$map_id;
    
    // second connection to the database to create the new database
    $conn = Database::connect();
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $sql = "CREATE DATABASE $dbname";
    $conn->exec($sql);

    Database::disconnect();

    // third connection to the database to create the tables necessary to the databse
    $conn = Database::connect($dbname);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $sql = "CREATE TABLE radar_values(
            x FLOAT,
            y FLOAT,
            val FLOAT,
            read_time DATETIME)";
    $conn->exec($sql);    
    $sql = "CREATE TABLE alien_location(
            x FLOAT,
            y FLOAT,
            colour ENUM('red', 'pink', 'teal', 'yellow', 'blue', 'green'),
            detect_time DATETIME
        )";
    $conn->exec($sql);
    $sql = "CREATE TABLE instructions(
            instruction INT,
            val FLOAT,
            executed BOOL,
            order_time DATETIME,
            execute_time DATETIME
        )";
    $conn->exec($sql);
    $sql = "CREATE TABLE building_location(
            x FLOAT,
            y FLOAT,
            detect_time DATETIME
        )";
    $conn->exec($sql);
    Database::disconnect();


?>