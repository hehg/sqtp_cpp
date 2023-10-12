#ifndef    SQ_STRATEGY_H
#define    SQ_STRATEGY_H

/**
 * @brief 这个框架的主要目的是提供给第三方使用，可供python等脚本语言调用
 *        对frame 内部c函数的封装
*/
//此文件接口尽量按c语言

//前置声明
class sq_strategy_rsp;


/**
 * @brief 请求对象,提供与交易相关的各种接口
 *        
 *      
*/
class sq_frame_caller
{
public:
    /**
     * @brief 打开策略框架对象
     * @param rsp 回报接收对象
     * @param cfg_path 存放配置文件的路径,如果为nullptr 
     *        默认使用 ../conf/config.xml
     * @return 0 成功  其它 失败 
     */
    int open(sq_strategy_rsp *rsp,const char* cfg_path);
    void run();
    // 关闭请求对象
    void close();

    //====行情类请求====//
    void sub_marketdata(const char *req);
    //====交易类请求====//
    int buy_open(int local_id, const char *contract, double price, int qty);
    int sell_open(int local_id, const char *contract, double price, int qty);
    int buy_close(int local_id, const char *contract, double price, int qty);
    int sell_close(int local_id, const char *contract, double price, int qty);

    int cancel_order(int local_id);
    //====查询类请求====//
    /**
     * 查询合约信息
     * @param market 市场名词
     *   市场包括:dce,shfe,sse,sze,czce,ine,cffex
     * @param contract 合约信息
     * @return 0 成功  其它 失败
     */
    int query_contract(const char *market, const char *contract);

    /**
     * @brief 通过err_code 获取错误的描述信息
     * @param err_code 上面函数调用的返回值
     * @return 函数的错误描述信息
     */
    const char *get_err_msg(int err_code);

    void callback(int tid,char*msg,int msg_size);
    sq_strategy_rsp *m_callback_ = nullptr; // 回调函数
};



/**
 * @brief 策略应答对象，为各种应答信息，通知信息提供回调接口
*/
class sq_strategy_rsp
{
public:
    virtual ~sq_strategy_rsp(){};

    /**
     * @brief 行情通知
     */
    virtual void on_market_data(
        const char *symbol,
        const char *market,
        double last_price,
        int match_qty,
        double turnover,
        int interest,
        double bid_price[5],int bid_qty[5],
        double ask_price[5],int ask_qty[5]
        ) = 0;
    
    /**
     * @brief 订单接受
     * @param local_id 订单本地编号
     * @param sys_id   交易所对订单的编号
     * @details 此订单被交易所接受时回调此函数
     */
    virtual void on_order_accepted(int local_id, const char *sys_id) = 0;
    virtual void on_order_matched(int local_id,const char*contract,
    double match_price,double match_qty) = 0;
    /**
     * 订单/撤单 被拒绝
     * @param local_id 订单本地编号
     * @param err_code 订单被拒绝的错误码
     * @param reason   订单被拒绝的原因
     * @note 撤单被拒绝时也回调此函数
     */
    virtual void on_order_rejected(int local_id, int err_code) = 0;
    virtual void on_order_canceled(int local_id) = 0;

    // 查询类
    virtual void on_query_contract(int result, const char *err_reson, const char *contract) = 0;
};

#endif