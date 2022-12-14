function onUpdateDatabase()
	Spdlog.info("Updating database to version 25 (boosted boss)")
	db.query([[
	CREATE TABLE IF NOT EXISTS `boosted_boss` (
		`boostname` TEXT,
		`date` varchar(250) NOT NULL DEFAULT '',
		`raceid` varchar(250) NOT NULL DEFAULT '',
		`looktype` int(11) NOT NULL DEFAULT "136",
		`lookfeet` int(11) NOT NULL DEFAULT "0",
		`looklegs` int(11) NOT NULL DEFAULT "0",
		`lookhead` int(11) NOT NULL DEFAULT "0",
		`lookbody` int(11) NOT NULL DEFAULT "0",
		`lookaddons` int(11) NOT NULL DEFAULT "0",
		`lookmount` int(11) DEFAULT "0",
		PRIMARY KEY (`date`)
	) AS SELECT 0 AS date, "default" AS boostname, 0 AS raceid]])
	return true
end
