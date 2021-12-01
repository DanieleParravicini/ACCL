#ifndef TCP_PACKETIZER_H
    #define TCP_PACKETIZER_H

    #define DATA_WIDTH 512
    #define PKT_CMD_DST_START                    0
    #define PKT_CMD_DST_END                      PKT_CMD_DST_START + 31
    #define PKT_CMD_LEN_START                    PKT_CMD_DST_END + 1
    #define PKT_CMD_LEN_END                      PKT_CMD_LEN_START + 31
    #define PKT_CMD_MPI_TAG_START                PKT_CMD_LEN_END + 1
    #define PKT_CMD_MPI_TAG_END                  PKT_CMD_MPI_TAG_START + 31
    #define PKT_CMD_SRC_RANK_START               PKT_CMD_MPI_TAG_END + 1
    #define PKT_CMD_SRC_RANK_END                 PKT_CMD_SRC_RANK_START + 31
    #define PKT_CMD_SEQ_NUM_START                PKT_CMD_SRC_RANK_END + 1
    #define PKT_CMD_SEQ_NUM_END                  PKT_CMD_SEQ_NUM_START + 31
    #define PKT_CMD_SIZE                         512 

    #define	TX_HANLDER_SESSION_START			 0
    #define	TX_HANLDER_SESSION_END				 TX_HANLDER_SESSION_START + 31
    #define	TX_HANLDER_LEN_START			 	 TX_HANLDER_SESSION_END + 1
    #define	TX_HANLDER_LEN_END				 	 TX_HANLDER_LEN_START + 31
    #define	TX_HANLDER_MAX_PKT_SIZE_START		 TX_HANLDER_LEN_END + 1
    #define	TX_HANLDER_MAX_PKT_SIZE_END			 TX_HANLDER_MAX_PKT_SIZE_START + 31
    #define TX_HANDLER_CMD_SIZE					 96
#endif