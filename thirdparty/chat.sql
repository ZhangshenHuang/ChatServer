--
-- The basic database table structure
-- You have to create a database, such as 'chat', and then run command 'source /your path.../chat.sql' 
--


--
-- Creating table `allgroup`
--

DROP TABLE IF EXISTS `allgroup`;
CREATE TABLE `allgroup` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `groupname` varchar(50) CHARACTER SET latin1 NOT NULL,
  `groupdesc` varchar(200) CHARACTER SET latin1 DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `groupname` (`groupname`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

--
-- Dumping data for table `allgroup`
--

LOCK TABLES `allgroup` WRITE;
INSERT INTO `allgroup` VALUES (1,'C++ chat project','My first chat project');
UNLOCK TABLES;

--
-- Table structure for table `friend`
--

DROP TABLE IF EXISTS `friend`;
CREATE TABLE `friend` (
  `userid` int(11) NOT NULL,
  `friendid` int(11) NOT NULL,
  KEY `userid` (`userid`,`friendid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `friend`
--

LOCK TABLES `friend` WRITE;
INSERT INTO `friend` VALUES (13,15),(13,21),(21,13),(21,15);
UNLOCK TABLES;

--
-- Table structure for table `groupuser`
--

DROP TABLE IF EXISTS `groupuser`;
CREATE TABLE `groupuser` (
  `groupid` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `grouprole` enum('creator','normal') CHARACTER SET latin1 DEFAULT NULL,
  KEY `groupid` (`groupid`,`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `groupuser`
--

LOCK TABLES `groupuser` WRITE;
INSERT INTO `groupuser` VALUES (1,13,'creator'),(1,21,'normal'),(1,19,'normal');
UNLOCK TABLES;

--
-- Table structure for table `offlinemessage`
--

DROP TABLE IF EXISTS `offlinemessage`;
CREATE TABLE `offlinemessage` (
  `userid` int(11) NOT NULL,
  `message` varchar(500) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `offlinemessage`
--

LOCK TABLES `offlinemessage` WRITE;
INSERT INTO `offlinemessage` VALUES (19,'{\"groupid\":1,\"id\":21,\"msg\":\"hello\",\"msgid\":10,\"name\":\"gao yang\",\"time\":\"2020-02-22 00:43:59\"}'),(19,'{\"groupid\":1,\"id\":21,\"msg\":\"helo!!!\",\"msgid\":10,\"name\":\"gao yang\",\"time\":\"2020-02-22 22:43:21\"}'),(19,'{\"groupid\":1,\"id\":13,\"msg\":\"hahahahaha\",\"msgid\":10,\"name\":\"zhang san\",\"time\":\"2020-02-22 22:59:56\"}'),(19,'{\"groupid\":1,\"id\":13,\"msg\":\"hahahahaha\",\"msgid\":10,\"name\":\"zhang san\",\"time\":\"2020-02-23 17:59:26\"}'),(19,'{\"groupid\":1,\"id\":21,\"msg\":\"wowowowowow\",\"msgid\":10,\"name\":\"gao yang\",\"time\":\"2020-02-23 17:59:34\"}');
UNLOCK TABLES;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) DEFAULT NULL,
  `password` varchar(50) DEFAULT NULL,
  `state` enum('online','offline') CHARACTER SET latin1 DEFAULT 'offline',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=utf8;

--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
INSERT INTO `user` VALUES (13,'zhang san','123456','offline'),(15,'li si','666666','offline'),(16,'liu shuo','123456','offline'),(18,'wu yang','123456','offline'),(19,'pi pi','123456','offline'),(21,'gao yang','123456','offline');
UNLOCK TABLES;


-- Dump completed on 2023-06-15
