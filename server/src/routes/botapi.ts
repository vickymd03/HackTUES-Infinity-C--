import { Router } from 'express';
import fileUpload from 'express-fileupload';

const router = Router();

router.use(fileUpload({

}));

router.get('/status', (req, res) => {
    res.send('Bot is kinda ok');
});

router.post('/image', (req, res) => {
    console.log(req.files);

    res.sendStatus(200);
});

export default router;
