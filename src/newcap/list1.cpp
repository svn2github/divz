/***************************************************************************/
bool FFMPEG_VideoWriter::initCapture(const QString &strFileName, float fFrameRate,
                                                                         const Vec2D<double> &v2dImageRes, bool bColor)
{
        QString strFile = strCapturePath + strFileName;
        // start from clean state
        deinitCapture();

        // create container format description
        m_pOutputFormat = guess_format(NULL, (const char*)strFile.toLatin1(), NULL);
    if (!m_pOutputFormat)
        {
                cout<<"FFMPEG_VideoWriter: Could not deduce output format from file extension: using MPEG"<<endl;
        m_pOutputFormat = guess_format("mpeg", NULL, NULL);
                if (!m_pOutputFormat)
                        cout<<"FFMPEG_VideoWriter: Could not find mpeg output format"<<endl;
    }

        // FIXME codec choice
        m_pOutputFormat->video_codec = CODEC_ID_RAWVIDEO;

        // select appropriate pixel format
        if(bColor)
        m_iInputPixFormat = PIX_FMT_RGB32_1;
        else
                m_iInputPixFormat = PIX_FMT_GRAY8;

    // allocate the output media context
    m_pFormatContext =  av_alloc_format_context();
    if (!m_pFormatContext)
        {
                cout<<"FFMPEG_VideoWriter: Memory error"<<endl;
                return false;
        }
   
    m_pFormatContext->oformat = m_pOutputFormat;
        _snprintf(m_pFormatContext->filename, sizeof(m_pFormatContext->filename), "%s", (const char*)strFile.toLatin1());

        ///////////////////// create stream and configure the codec context //////////////////////////
     m_pVideoStreamFFMPEG =  av_new_stream(m_pFormatContext, 0);
         if(!m_pVideoStreamFFMPEG)
         {
                 cout<<"FFMPEG_VideoWriter: Could not alloc video stream"<<endl;
         }
        AVCodecContext* pCodecContext = m_pVideoStreamFFMPEG->codec;
        pCodecContext->codec_id = m_pOutputFormat->video_codec;
        pCodecContext->codec_type = CODEC_TYPE_VIDEO;
       
        pCodecContext->bit_rate = v2dImageRes.m_x * v2dImageRes.m_y * 4 * fFrameRate * 8;
        pCodecContext->width = (int)v2dImageRes.m_x;
        pCodecContext->height = (int)v2dImageRes.m_y;

        pCodecContext->time_base.num = 1;
        pCodecContext->time_base.den = fFrameRate;
   
        pCodecContext->pix_fmt = (PixelFormat)m_iInputPixFormat;
       
        // some formats want stream headers to be separate
    if(m_pFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
        pCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;

        //init output params
        av_set_parameters(m_pFormatContext,NULL);

        //dump_format(m_pFormatContext,0,(const char*)strFile.toLatin1(),1);

        /////////////////////// open video file /////////////////////////////////////////
    // find encoder
    AVCodec *pCodec = avcodec_find_encoder(pCodecContext->codec_id);
    if (!pCodec)
        {
                cout<<"FFMPEG_VideoWriter: Codec not found"<<endl;
        return false;
    }

    // open the codec
    if (avcodec_open(pCodecContext, pCodec) < 0)
        {
                cout<<"FFMPEG_VideoWriter: Could not open codec"<<endl;
        return false;
    }

        // create data structure for source picture
    m_pInputPicture = allocPicture(pCodecContext->pix_fmt, pCodecContext->width, pCodecContext->height, false);
    if (!m_pInputPicture)
        {
                return false;
                cout<<"FFMPEG_VideoWriter: Could not allocate picture"<<endl;
        }
        // create memory for compressed image, assume 32 bit pixel maximum
        m_uiCompressedImageBufSize = v2dImageRes.m_x*v2dImageRes.m_y*4;
        m_pCompressedImageBuf = (uint8_t *) av_malloc(m_uiCompressedImageBufSize);

        // open the output file
        if(url_fopen(&m_pFormatContext->pb, m_pFormatContext->filename, URL_WRONLY) < 0)
        {
                cout<<"FFMPEG_VideoWriter: Could not open file"<<endl;
                return false;
        }

        // write stream header
        av_write_header(m_pFormatContext);

        return true;
}

/***************************************************************************/
bool FFMPEG_VideoWriter::writeImage(vil_image_resource_sptr pImg)
{
        if(m_pFormatContext)
        {
                AVCodecContext* pCodecContext = m_pVideoStreamFFMPEG->codec;
                // set rgba input image
                if(pCodecContext->pix_fmt == PIX_FMT_RGB32_1 && pImg->nplanes() == 4 &&
                        pCodecContext->width == pImg->ni() &&
                        pCodecContext->height == pImg->nj())
                {
                        avpicture_fill((AVPicture*)m_pInputPicture,
                                           (uint8_t*)((vil_image_view<vxl_byte>)pImg->get_view()).top_left_ptr(),
                                                   PIX_FMT_RGB32_1,
                                                   pImg->ni(), pImg->nj());

                }
                // set monochrome input image
                else if(pCodecContext->pix_fmt == PIX_FMT_GRAY8 && pImg->nplanes() == 1 &&
                            pCodecContext->width == pImg->ni() &&
                            pCodecContext->height == pImg->nj())
                {
                        avpicture_fill((AVPicture*)m_pInputPicture,
                                           (uint8_t*)((vil_image_view<vxl_byte>)pImg->get_view()).top_left_ptr(),
                                                   PIX_FMT_GRAY8,
                                                   pImg->ni(), pImg->nj());
                }
                else
                        return false;

                // compress image
                int iOutSize = avcodec_encode_video(pCodecContext, m_pCompressedImageBuf,
                                                                                        m_uiCompressedImageBufSize, m_pInputPicture);
                // serialize data
                if(iOutSize > 0)
                {
                        // create data package
            AVPacket pkt;
            av_init_packet(&pkt);

            if (pCodecContext->coded_frame->pts != AV_NOPTS_VALUE)
                pkt.pts= av_rescale_q(pCodecContext->coded_frame->pts,
                                                                          pCodecContext->time_base,
                                                                          m_pVideoStreamFFMPEG->time_base);

            if(pCodecContext->coded_frame->key_frame)
                pkt.flags |= PKT_FLAG_KEY;
           
                        pkt.stream_index= m_pVideoStreamFFMPEG->index;
            pkt.data= m_pCompressedImageBuf;
            pkt.size= iOutSize;

            // write the package into file
            av_interleaved_write_frame(m_pFormatContext, &pkt);
                }
        }// end format context

        return true;
}


