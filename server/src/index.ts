import express from 'express'
import morgan from 'morgan'

import config from './config'
import routes from './routes'

const app = express();

app.set('view-engine', 'ejs');
app.use(morgan('dev'));
app.use(routes)

app.listen(config.port, () => {
    console.log(`Listening on port ${config.port} with the following settings`);
    console.log(config);
});
