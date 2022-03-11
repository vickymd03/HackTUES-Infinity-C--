import express from 'express'

import config from './config'
import routes from './routes'

const app = express();

app.set('view-engine', 'ejs');
app.use(routes)

app.listen(config.port, () => {
    console.log(`Listening on port ${config.port} with the following settings`);
    console.log(config);
});
