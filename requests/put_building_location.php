<?php

    include 'db_connect.php';

    $orientation = $_POST['orientation'];

    if(($_POST['distance']<=50)&&($_POST['distance']>9)){
        switch($orientation){
            case 0:
                $x = $_POST['x'] + $_POST['distance'];
                $y = $_POST['y'];
                break;
            case 1:
                $x = $_POST['x'] - $_POST['distance'];
                $y = $_POST['y'];
                break;
            case 2: 
                $x = $_POST['x'];
                $y = $_POST['y'] - $_POST['distance'];
                break;
            case 3:
                $x = $_POST['x'];
                $y = $_POST['y'] + $_POST['distance'];
                break;
            default:
                exit(1);
        }

        if($x > 500 || $y > 500){
            exit(1);
        }


        $time = date("Y-m-d H:i:s");
        $map_id = "map_".getMap();

        // connect to the database
        $conn = Database::connect($map_id);
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

        $sql = "INSERT INTO building_location(x, y, colour, detect_time) 
                VALUE('$x', '$y', '$colour', '$time')";
        $conn->exec($sql);

        Database::disconnect();
    }

?>