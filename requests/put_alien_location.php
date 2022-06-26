<?php

    include 'db_connect.php';

    $orientation = $_POST['orientation'];
    $distance = $_POST['distance'];

    if(($distance<=50)&&($distance>14)){
        switch($orientation){
            case 0:
                $x = $_POST['x'] + $distance;
                $y = $_POST['y'];
                break;
            case 1:
                $x = $_POST['x'] - $distance;
                $y = $_POST['y'];
                break;
            case 2: 
                $x = $_POST['x'];
                $y = $_POST['y'] - $distance;
                break;
            case 3:
                $x = $_POST['x'];
                $y = $_POST['y'] + $distance;
                break;
            default:
                exit(1);
        }

        if($x > 500 || $y > 500){
            exit(1);
        }

        $colour = $_POST['colour'];
        $time = date("Y-m-d H:i:s");
        $map_id = "map_".getMap();

        // connect to the database
        $conn = Database::connect($map_id);
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);


        $sql = "INSERT INTO alien_location(x, y, colour, detect_time) 
                VALUE('$x', '$y', '$colour', '$time')";
        $conn->exec($sql);

        Database::disconnect();
    }


?>