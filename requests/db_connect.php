<?php

    class Database{
        private static $dbHost = 'localhost';
        private static $dbUserName = 'PHPScript';
        private static $dbPassword = 'php_access';

        private static $conn = null;

        public function __construct(){
            die('Init function is not allowed');
        }

        public static function connect($dbName=null) {
            if(null == self::$conn){
                try{
                    if ($dbName==null){
                        self::$conn = new PDO("mysql:host=".self::$dbHost, self::$dbUserName, self::$dbPassword);
                    }
                    else{
                        self::$conn = new PDO("mysql:host=".self::$dbHost.";"."dbname=".$dbName, self::$dbUserName, self::$dbPassword);
                    }
                }
                catch(PDOException $e){
                    die($e->getMessage());
                }
            }
            return self::$conn;
        }

        public static function disconnect(){
            self::$conn = null;
        }
    }

    function getMap(){
        $conn = Database::connect('map_list');
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        $sql = "SELECT id FROM maps ORDER BY assign_time DESC LIMIT 1";
        $qr = $conn->query($sql);
        Database::disconnect();
        foreach($qr as $row){
            $res = $row['id'];
            break;
        }
        return $res;
    }


?>