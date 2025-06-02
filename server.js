const express = require('express');
const multer = require('multer');
const path = require('path');
const fs = require('fs');

const app = express();
const port = 3000;

const storage = multer.diskStorage({
	destination: function (req, file, cb) {
	const uploadDir = '';
	if(!fs.existsSync(uploadDir)) {
		fs.mkdirSync(uploadDir);
	}
	cb(null, uploadDir);
	},
	filename: function (req, file, cb){
	const uniqueSuffix = Date.now() + '-' + Math.round(Math.random()*1E9);
	cb(null, file.fieldname + '-' + uniqueSuffix + path.extname(file.originalname));
	}
 });
	const upload = multer({storage:storage});
	app.post(uploadDir,upload.single('imagen'),(req,res) => {
	 //console.log('Headers:', req.headers);
	 //console.log('Body:', req.body);
	 //console.log('Files received:', req.file);

		if(req.file) {
		 console.log('Image receive and saved:', req.file.path);
		 res.status(200).json({message: 'Upload of image done.', filename: req.file.filename});
		}
		else {
		 console.log('Image not received');
		 res.status(400).send('Image not received');
		}
	});

	app.use(uploadDir, express.static(path.join(__dirname,'')));

	//app.use(express.json());
	//app.use(express.urlencoded({extended:true}));

	app.get('/',(req,res) => {
	 res.send('HTTP server online');
	});

	app.listen(port, () => {
		console.log('Server http://localhost:3000');
		console.log('Endpoint http://localhost:3000',uploadDir);
	});
