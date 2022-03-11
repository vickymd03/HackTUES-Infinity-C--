import { Router } from 'express';
import fileUpload from 'express-fileupload';

import config from '../config'

const router = Router();

router.use(fileUpload({}));

router.use((req, res, next) => {
    setTimeout(next, config.simulatedLatency);
});

router.get('/status', (req, res) => {
    res.send('Bot is kinda ok');
});

router.post('/image', (req, res) => {
    console.log(req.files);

    res.sendStatus(200);
});

export default router;
