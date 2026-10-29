#include <string>

using namespace std;

class Admin : public TaiKhoan {
public:
    Admin();
    Admin(const string& id, const string& username, const string& passwordHash);
    ~Admin() override = default;

    void show() const override;
    string xuatChuoiFile() const override;
};
