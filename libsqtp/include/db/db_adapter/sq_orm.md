#接口参考

//inset
affected, err := engine.Insert(&user)

//get
has, err := engine.Get(&user)
// SELECT * FROM user LIMIT 1

has, err := engine.Where("name = ?", name).Desc("id").Get(&user)
// SELECT * FROM user WHERE name = ? ORDER BY id DESC LIMIT 1

var name string
has, err := engine.Table(&user).Where("id = ?", id).Cols("name").Get(&name)
// SELECT name FROM user WHERE id = ?

var id int64
has, err := engine.Table(&user).Where("name = ?", name).Cols("id").Get(&id)
has, err := engine.SQL("select id from user").Get(&id)
// SELECT id FROM user WHERE name = ?
