// ==========================================================================
// MeoLogi - app.js (Quản lý giao diện & Kết nối API Backend C++)
// Tuân thủ toàn bộ yêu cầu trong web/README.md & Không dùng mock data riêng
// ==========================================================================

import { CAT_ICONS } from './src/catData.js';

// Chuẩn hóa tiếng Việt phục vụ tìm kiếm thông minh không dấu
function removeVietnameseTones(str) {
  if (!str) return '';
  return String(str)
    .normalize('NFD')
    .replace(/[\u0300-\u036f]/g, '')
    .replace(/[đĐ]/g, 'd')
    .toLowerCase()
    .trim();
}

class MeoLogiApp {
  constructor() {
    this.apiBase = '';
    this.token = sessionStorage.getItem('meologi_token') || '';
    this.currentUser = JSON.parse(sessionStorage.getItem('meologi_user') || 'null');
    this.currentRole = 'customer';

    // Dữ liệu bộ nhớ khách
    this.allProducts = [];
    this.products = [];
    this.cart = JSON.parse(localStorage.getItem('meologi_cart') || '[]');
    this.currentCategory = 'all';
    this.currentSort = 0;
    this.currentSearchKeyword = '';
    this.searchDebounce = null;

    // Dữ liệu quản trị
    this.adminOrders = [];
    this.adminWarehouses = [];
    this.adminSuppliers = [];
    this.adminRevenues = [];
    this.adminUsers = [];
    this.adminReviews = [];
  }

  // ------------------------------------------------------------------------
  // Khởi tạo ứng dụng & Icon mèo inline
  // ------------------------------------------------------------------------
  async init() {
    this.mountCatIcons();
    this.updateCartBadge();
    this.initCustomSortDropdown();

    // Đóng dropdown tìm kiếm khi click ngoài
    document.addEventListener('click', (e) => {
      const wrap = document.querySelector('.search-wrap');
      if (wrap && !wrap.contains(e.target)) {
        this.closeSearchDropdown();
      }
    });

    // Lắng nghe sự kiện gõ username để kiểm tra trùng qua cây Trie trong thời gian thực
    const usrInput = document.getElementById('usr-username');
    if (usrInput) {
      let debounceTimer = null;
      usrInput.addEventListener('input', () => {
        clearTimeout(debounceTimer);
        debounceTimer = setTimeout(() => {
          this.checkUsernameRealtime(usrInput.value.trim());
        }, 300);
      });
    }

    // Kiểm tra phiên đăng nhập đã lưu
    if (this.token && this.currentUser) {
      const ok = await this.verifySession();
      if (ok) {
        if (this.currentUser.role === 'ADMIN') {
          this.switchView('view-admin');
          this.showAdminTab('orders');
        } else {
          this.switchView('view-customer');
          this.showCustTab('store');
        }
        return;
      }
    }

    // Chưa đăng nhập -> vào màn hình đăng nhập
    this.switchView('view-login');
  }

  // Gắn các icon mèo inline (base64 data-URI, không tải file ngoài)
  mountCatIcons() {
    const setImg = (id, src) => {
      const el = document.getElementById(id);
      if (el && src) el.innerHTML = `<img src="${src}" alt="Cat" />`;
    };

    setImg('login-cat-icon', CAT_ICONS.happy);
    setImg('cust-brand-cat', CAT_ICONS.happy);
    setImg('admin-brand-cat', CAT_ICONS.happy);

    setImg('cat-icon-all', CAT_ICONS.paws);
    setImg('cat-icon-thuong', CAT_ICONS.heart);
    setImg('cat-icon-tuoisong', CAT_ICONS.drink);
    setImg('cat-icon-congnghe', CAT_ICONS.sparkle);
    setImg('cat-icon-giadung', CAT_ICONS.loaf);

    setImg('adm-icon-orders', CAT_ICONS.envelope);
    setImg('adm-icon-products', CAT_ICONS.sparkle);
    setImg('adm-icon-warehouse', CAT_ICONS.alert);
    setImg('adm-icon-revenues', CAT_ICONS.thumbsup);
    setImg('adm-icon-users', CAT_ICONS.wave);
    setImg('adm-icon-reviews', CAT_ICONS.look);

    setImg('kpi-icon-revenue', CAT_ICONS.thumbsup);
    setImg('kpi-icon-orders', CAT_ICONS.envelope);
    setImg('kpi-icon-average', CAT_ICONS.sparkle);
    setImg('kpi-icon-days', CAT_ICONS.look);
  }

  // Helper định dạng tiền tệ
  formatCurrency(amount) {
    if (typeof amount !== 'number') amount = Number(amount) || 0;
    return amount.toLocaleString('vi-VN') + ' đ';
  }

  // Chuyển đổi màn hình độc quyền (Exclusive View Switching)
  switchView(viewId) {
    document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));
    const target = document.getElementById(viewId);
    if (target) target.classList.add('active');
  }

  // Hiển thị thông báo Toast
  showToast(message, isError = false) {
    const container = document.getElementById('toast-container');
    if (!container) return;
    const toast = document.createElement('div');
    toast.className = 'toast';
    if (isError) {
      toast.style.borderColor = 'var(--princeton-orange)';
    }
    toast.innerHTML = `
      <div style="width: 24px; height: 24px; flex-shrink: 0;"><img src="${isError ? CAT_ICONS.alert : CAT_ICONS.happy}" style="width:100%;height:100%;object-fit:contain;" /></div>
      <span>${message}</span>
    `;
    container.appendChild(toast);
    setTimeout(() => {
      toast.remove();
    }, 4000);
  }

  // ------------------------------------------------------------------------
  // Xác thực & Phiên đăng nhập
  // ------------------------------------------------------------------------
  setRole(role) {
    this.currentRole = role;
    const btnCust = document.getElementById('role-btn-cust');
    const btnAdmin = document.getElementById('role-btn-admin');
    const u = document.getElementById('login-username');
    const p = document.getElementById('login-password');
    if (role === 'admin') {
      btnAdmin.classList.add('active');
      btnCust.classList.remove('active');
      if (u) u.placeholder = 'Tên đăng nhập quản trị...';
      if (p) p.placeholder = 'Mật khẩu quản trị...';
    } else {
      btnCust.classList.add('active');
      btnAdmin.classList.remove('active');
      if (u) u.placeholder = 'Tên đăng nhập khách hàng...';
      if (p) p.placeholder = 'Mật khẩu...';
    }
  }

  async verifySession() {
    try {
      const res = await fetch(`${this.apiBase}/api/me`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (res.ok) {
        const data = await res.json();
        if (data.success && data.user) {
          this.currentUser = data.user;
          sessionStorage.setItem('meologi_user', JSON.stringify(this.currentUser));
          return true;
        }
      }
    } catch (e) {}
    this.clearSession();
    return false;
  }

  clearSession() {
    this.token = '';
    this.currentUser = null;
    sessionStorage.removeItem('meologi_token');
    sessionStorage.removeItem('meologi_user');
  }

  async handleLogin(e) {
    e.preventDefault();
    const username = document.getElementById('login-username').value.trim();
    const password = document.getElementById('login-password').value;

    try {
      const res = await fetch(`${this.apiBase}/api/login`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username, password, role: this.currentRole })
      });
      const data = await res.json();
      if (data.success && data.token) {
        this.token = data.token;
        this.currentUser = data.user;
        sessionStorage.setItem('meologi_token', this.token);
        sessionStorage.setItem('meologi_user', JSON.stringify(this.currentUser));
        this.showToast(`Chào mừng ${data.user.hoTen || data.user.username}!`);

        if (data.user.role === 'ADMIN') {
          this.switchView('view-admin');
          this.showAdminTab('orders');
        } else {
          this.switchView('view-customer');
          this.showCustTab('store');
        }
      } else {
        this.showToast(data.message || 'Đăng nhập thất bại!', true);
      }
    } catch (err) {
      this.showToast('Không thể kết nối đến máy chủ C++.', true);
    }
  }

  async logout() {
    try {
      await fetch(`${this.apiBase}/api/logout`, {
        method: 'POST',
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
    } catch (e) {}
    this.clearSession();
    this.switchView('view-login');
    this.showToast('Đã đăng xuất khỏi hệ thống.');
  }

  openRegisterModal() {
    this.isRegisterMode = true;
    this.usernameExists = false;
    const usr = document.getElementById('usr-id');
    if (usr) usr.value = '';
    document.getElementById('user-form-title').textContent = 'Đăng Ký Tài Khoản Khách Hàng';
    document.getElementById('usr-role').value = 'CUSTOMER';
    document.getElementById('usr-role').disabled = true;
    const usrName = document.getElementById('usr-username');
    if (usrName) {
      usrName.value = '';
      usrName.readOnly = false;
      usrName.style.borderColor = '';
    }
    const feedback = document.getElementById('usr-username-feedback');
    if (feedback) { feedback.style.display = 'none'; feedback.textContent = ''; }
    document.getElementById('usr-password-group').style.display = 'block';
    document.getElementById('usr-password').value = '';
    document.getElementById('usr-password').required = true;
    document.getElementById('usr-hoten').value = '';
    document.getElementById('usr-sdt').value = '';
    document.getElementById('usr-diachi').value = '';
    const submitBtn = document.querySelector('#form-user button[type="submit"]');
    if (submitBtn) submitBtn.textContent = 'Đăng Ký Tài Khoản';
    this.openModal('modal-user-form');
  }

  // ------------------------------------------------------------------------
  // Khách hàng: Cửa hàng & Tìm kiếm thông minh
  // ------------------------------------------------------------------------
  async showCustTab(tab) {
    const navStore = document.getElementById('cust-nav-store');
    const navOrders = document.getElementById('cust-nav-orders');
    const tabStore = document.getElementById('cust-tab-store');
    const tabOrders = document.getElementById('cust-tab-orders');

    if (tab === 'orders') {
      navOrders.classList.add('btn-primary');
      navOrders.classList.remove('btn-outline');
      navStore.classList.add('btn-outline');
      navStore.classList.remove('btn-primary');
      if (tabStore) tabStore.classList.add('hidden');
      if (tabOrders) tabOrders.classList.remove('hidden');
      await this.loadCustOrders();
    } else {
      navStore.classList.add('btn-primary');
      navStore.classList.remove('btn-outline');
      navOrders.classList.add('btn-outline');
      navOrders.classList.remove('btn-primary');
      if (tabStore) tabStore.classList.remove('hidden');
      if (tabOrders) tabOrders.classList.add('hidden');
      await this.loadProducts();
    }
  }

  async loadProducts() {
    try {
      const res = await fetch(`${this.apiBase}/api/products?sort=${this.currentSort}`);
      if (res.ok) {
        const raw = await res.json();
        this.allProducts = (raw || []).map(p => ({
          ...p,
          tenSP: String(p.tenSP || '').normalize('NFC'),
          danhMuc: String(p.danhMuc || '').normalize('NFC')
        }));
        this.applyFilterAndRender();
        const countAll = document.getElementById('count-all-products');
        if (countAll) countAll.textContent = this.allProducts.length;
      }
    } catch (e) {
      console.error('Lỗi nạp sản phẩm:', e);
    }
  }

  filterCategory(category, buttonEl) {
    this.currentCategory = category;
    if (buttonEl) {
      document.querySelectorAll('.cat-pill').forEach(b => b.classList.remove('active'));
      buttonEl.classList.add('active');
    }
    this.applyFilterAndRender();
  }

  handleSortChange(sortVal) {
    this.currentSort = parseInt(sortVal, 10) || 0;
    const label = document.getElementById('sort-custom-label');
    const select = document.getElementById('sort-products-select');
    const menu = document.getElementById('sort-custom-menu');
    if (select && select.value !== String(this.currentSort)) {
      select.value = String(this.currentSort);
    }
    if (menu) {
      menu.querySelectorAll('.custom-dropdown-item').forEach(item => {
        if (item.getAttribute('data-value') === String(this.currentSort)) {
          item.classList.add('active');
          if (label) label.textContent = item.textContent.trim();
        } else {
          item.classList.remove('active');
        }
      });
    }
    this.loadProducts();
  }

  initCustomSortDropdown() {
    const dropdown = document.getElementById('sort-custom-dropdown');
    const btn = document.getElementById('sort-custom-btn');
    const label = document.getElementById('sort-custom-label');
    const menu = document.getElementById('sort-custom-menu');
    const select = document.getElementById('sort-products-select');
    if (!dropdown || !btn || !menu) return;

    btn.addEventListener('click', (e) => {
      e.stopPropagation();
      const isOpen = dropdown.classList.toggle('open');
      btn.setAttribute('aria-expanded', isOpen ? 'true' : 'false');
    });

    menu.querySelectorAll('.custom-dropdown-item').forEach(item => {
      item.addEventListener('click', (e) => {
        e.stopPropagation();
        const val = item.getAttribute('data-value');
        const text = item.textContent.trim();
        if (label) label.textContent = text;
        if (select) select.value = val;
        menu.querySelectorAll('.custom-dropdown-item').forEach(i => i.classList.remove('active'));
        item.classList.add('active');
        dropdown.classList.remove('open');
        btn.setAttribute('aria-expanded', 'false');
        this.handleSortChange(val);
      });
    });

    document.addEventListener('click', (e) => {
      if (!dropdown.contains(e.target)) {
        dropdown.classList.remove('open');
        btn.setAttribute('aria-expanded', 'false');
      }
    });

    document.addEventListener('keydown', (e) => {
      if (e.key === 'Escape' && dropdown.classList.contains('open')) {
        dropdown.classList.remove('open');
        btn.setAttribute('aria-expanded', 'false');
      }
    });
  }

  handleSearch(e) {
    const rawVal = e.target.value;
    this.currentSearchKeyword = rawVal.trim();
    const clearBtn = document.getElementById('cust-search-clear');
    if (clearBtn) {
      if (rawVal.length > 0) {
        clearBtn.classList.remove('hidden');
      } else {
        clearBtn.classList.add('hidden');
      }
    }

    // 0ms instant filter trên client-side
    this.applyFilterAndRender();
    this.renderSearchSuggestions();

    // Debounce gọi Backend API
    clearTimeout(this.searchDebounce);
    if (this.currentSearchKeyword.length > 0) {
      this.searchDebounce = setTimeout(() => {
        this.fetchBackendSearch(this.currentSearchKeyword);
      }, 250);
    }
  }

  handleSearchKey(e) {
    if (e.key === 'Enter') {
      this.submitSearch();
    } else if (e.key === 'Escape') {
      this.clearSearch();
    }
  }

  submitSearch() {
    this.closeSearchDropdown();
    this.applyFilterAndRender();
  }

  clearSearch() {
    const input = document.getElementById('cust-search-input');
    if (input) input.value = '';
    const clearBtn = document.getElementById('cust-search-clear');
    if (clearBtn) clearBtn.classList.add('hidden');
    this.currentSearchKeyword = '';
    this.closeSearchDropdown();
    this.applyFilterAndRender();
  }

  selectSuggestion(maSP) {
    this.closeSearchDropdown();
    this.openProductDetail(maSP);
  }

  applyFilterAndRender() {
    let list = this.allProducts;
    if (this.currentSearchKeyword) {
      const normKey = removeVietnameseTones(this.currentSearchKeyword);
      const kws = normKey.split(/\s+/).filter(Boolean);
      list = list.filter(item => {
        const nTen = removeVietnameseTones(item.tenSP || '');
        const nMa = removeVietnameseTones(item.maSP || '');
        const nDm = removeVietnameseTones(item.danhMuc || '');
        const nLoai = removeVietnameseTones(item.loaiSP || '');
        return kws.every(k => nTen.includes(k) || nMa.includes(k) || nDm.includes(k) || nLoai.includes(k));
      });
    } else if (this.currentCategory !== 'all') {
      list = list.filter(p => p.loaiSP === this.currentCategory);
    }

    this.products = list;
    this.renderProductGrid();
  }

  renderSearchSuggestions() {
    const dropdown = document.getElementById('cust-search-dropdown');
    if (!dropdown) return;
    if (!this.currentSearchKeyword) {
      dropdown.style.display = 'none';
      return;
    }
    const normKey = removeVietnameseTones(this.currentSearchKeyword);
    const kws = normKey.split(/\s+/).filter(Boolean);
    const matches = this.allProducts.filter(p => {
      const nTen = removeVietnameseTones(p.tenSP || '');
      const nMa = removeVietnameseTones(p.maSP || '');
      const nDm = removeVietnameseTones(p.danhMuc || '');
      return kws.every(k => nTen.includes(k) || nMa.includes(k) || nDm.includes(k));
    }).slice(0, 6);

    if (matches.length === 0) {
      dropdown.innerHTML = `<div style="padding: 12px; font-size: 13px; text-align: center; color: var(--space-indigo-light);">Không tìm thấy sản phẩm phù hợp.</div>`;
    } else {
      dropdown.innerHTML = matches.map(p => `
        <div class="suggestion-item" onclick="App.selectSuggestion('${p.maSP}')">
          <div style="flex: 1;">
            <div style="font-weight: 700; font-size: 13.5px; color: var(--space-indigo);">${p.tenSP}</div>
            <div style="font-size: 11.5px; color: var(--space-indigo-light);">${p.maSP} • ${p.danhMuc} • Tồn: ${p.soLuongTon}</div>
          </div>
          <div style="font-weight: 800; color: var(--princeton-orange); font-size: 13px;">${this.formatCurrency(p.donGia)}</div>
        </div>
      `).join('');
    }
    dropdown.style.display = 'block';
  }

  closeSearchDropdown() {
    const dropdown = document.getElementById('cust-search-dropdown');
    if (dropdown) dropdown.style.display = 'none';
  }

  async fetchBackendSearch(keyword) {
    try {
      const res = await fetch(`${this.apiBase}/api/products/search?q=${encodeURIComponent(keyword)}`);
      if (res.ok) {
        const backendRes = await res.json();
        if (backendRes && backendRes.length > 0 && this.currentSearchKeyword === keyword) {
          const map = new Map();
          [...this.products, ...backendRes].forEach(p => map.set(p.maSP, p));
          this.products = Array.from(map.values());
          this.renderProductGrid();
        }
      }
    } catch (e) {}
  }

  renderProductGrid() {
    const container = document.getElementById('product-grid');
    if (!container) return;

    if (this.products.length === 0) {
      container.innerHTML = `
        <div class="empty-state glass-panel" style="grid-column: 1 / -1;">
          <div class="empty-cat-img"><img src="${CAT_ICONS.question}" alt="Chưa có sản phẩm" /></div>
          <h4 class="empty-title">Không tìm thấy sản phẩm nào!</h4>
          <p class="empty-sub">Thử nhập từ khóa khác hoặc chuyển danh mục xem sao nhé.</p>
        </div>
      `;
      return;
    }

    container.innerHTML = this.products.map(p => `
      <div class="product-card glass-panel">
        <div class="product-placeholder" onclick="App.openProductDetail('${p.maSP}')" style="cursor: pointer;" title="Nhấn xem thông số chi tiết">
          <svg width="48" height="48" viewBox="0 0 24 24" fill="none" stroke="var(--space-indigo-light)" stroke-width="1.8">
            <path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z"></path>
            <polyline points="3.27 6.96 12 12.01 20.73 6.96"></polyline>
            <line x1="12" y1="22.08" x2="12" y2="12"></line>
          </svg>
          <span class="stock-tag">Tồn: ${p.soLuongTonKho}</span>
        </div>
        <span class="product-cat-name">${p.danhMuc}</span>
        <h3 class="product-title" onclick="App.openProductDetail('${p.maSP}')" style="cursor: pointer;">${p.tenSP}</h3>
        <div class="product-bottom-row">
          <span class="product-price">${this.formatCurrency(p.donGia)}</span>
          <div style="display: flex; gap: 6px;">
            <button type="button" class="btn btn-outline btn-sm" onclick="App.openProductDetail('${p.maSP}')" title="Xem chi tiết đa hình">Chi tiết</button>
            <button type="button" class="btn btn-primary btn-sm" onclick="App.addToCart('${p.maSP}')" ${p.soLuongTonKho <= 0 ? 'disabled' : ''}>+ Giỏ</button>
          </div>
        </div>
      </div>
    `).join('');
  }

  // ------------------------------------------------------------------------
  // Giỏ hàng & Thanh toán
  // ------------------------------------------------------------------------
  addToCart(productId) {
    const sp = this.allProducts.find(p => p.maSP === productId);
    if (!sp) return;
    const item = this.cart.find(c => c.productId === productId);
    if (item) {
      if (item.quantity + 1 > sp.soLuongTonKho) {
        this.showToast('Kho không đủ hàng để thêm tiếp!', true);
        return;
      }
      item.quantity += 1;
    } else {
      this.cart.push({ productId, quantity: 1 });
    }
    localStorage.setItem('meologi_cart', JSON.stringify(this.cart));
    this.updateCartBadge();
    this.showToast(`Đã thêm ${sp.tenSP} vào giỏ hàng!`);
  }

  updateCartBadge() {
    const badge = document.getElementById('cart-badge');
    const totalQty = this.cart.reduce((sum, item) => sum + item.quantity, 0);
    if (badge) badge.textContent = totalQty;
  }

  openCartModal() {
    this.renderCartModal();
    this.openModal('modal-cart');
  }

  renderCartModal() {
    const container = document.getElementById('cart-items-list');
    const totalEl = document.getElementById('cart-total-amount');
    if (!container) return;

    if (this.cart.length === 0) {
      container.innerHTML = `
        <div class="empty-state">
          <div class="empty-cat-img"><img src="${CAT_ICONS.sleep}" alt="Giỏ trống" /></div>
          <h4 class="empty-title">Giỏ hàng của bạn đang trống!</h4>
          <p class="empty-sub">Chưa có đơn nào, mèo đang chờ bạn đặt hàng.</p>
        </div>
      `;
      if (totalEl) totalEl.textContent = '0 đ';
      return;
    }

    let total = 0;
    container.innerHTML = this.cart.map(c => {
      const sp = this.allProducts.find(p => p.maSP === c.productId);
      if (!sp) return '';
      const subtotal = sp.donGia * c.quantity;
      total += subtotal;
      return `
        <div style="display: flex; justify-content: space-between; align-items: center; padding: 10px 0; border-bottom: 1px solid var(--jasmine-light);">
          <div>
            <div style="font-weight: 700; font-size: 14px;">${sp.tenSP}</div>
            <div style="font-size: 12px; color: var(--space-indigo-light);">${this.formatCurrency(sp.donGia)} x ${c.quantity}</div>
          </div>
          <div style="display: flex; align-items: center; gap: 8px;">
            <span style="font-weight: 800; color: var(--princeton-orange);">${this.formatCurrency(subtotal)}</span>
            <button type="button" class="btn btn-outline btn-sm" onclick="App.removeCartItem('${c.productId}')">✕</button>
          </div>
        </div>
      `;
    }).join('');

    if (totalEl) totalEl.textContent = this.formatCurrency(total);

    if (this.currentUser) {
      const nameInput = document.getElementById('ship-name');
      const phoneInput = document.getElementById('ship-phone');
      const addrInput = document.getElementById('ship-address');
      if (nameInput && !nameInput.value) nameInput.value = this.currentUser.hoTen || '';
      if (phoneInput && !phoneInput.value) phoneInput.value = this.currentUser.soDienThoai || '';
      if (addrInput && !addrInput.value) addrInput.value = this.currentUser.diaChi || '';
    }
  }

  removeCartItem(productId) {
    this.cart = this.cart.filter(c => c.productId !== productId);
    localStorage.setItem('meologi_cart', JSON.stringify(this.cart));
    this.updateCartBadge();
    this.renderCartModal();
  }

  async handleCheckout(e) {
    e.preventDefault();
    if (this.cart.length === 0) {
      this.showToast('Giỏ hàng trống!', true);
      return;
    }

    const customerName = document.getElementById('ship-name').value;
    const phone = document.getElementById('ship-phone').value;
    const address = document.getElementById('ship-address').value;

    let successCount = 0;
    for (const item of this.cart) {
      try {
        const res = await fetch(`${this.apiBase}/api/orders`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${this.token}`
          },
          body: JSON.stringify({
            productId: item.productId,
            quantity: item.quantity,
            customerName,
            phone,
            address
          })
        });
        const data = await res.json();
        if (data.success) successCount++;
      } catch (err) {}
    }

    if (successCount > 0) {
      this.cart = [];
      localStorage.removeItem('meologi_cart');
      this.updateCartBadge();
      this.closeModal('modal-cart');
      this.showToast(`Đã tạo thành công ${successCount} đơn hàng vào hàng đợi!`);
      this.showCustTab('orders');
    } else {
      this.showToast('Đặt hàng thất bại. Vui lòng thử lại!', true);
    }
  }

  // ------------------------------------------------------------------------
  // Khách hàng: Lịch sử & Tra cứu đơn BST
  // ------------------------------------------------------------------------
  async loadCustOrders() {
    try {
      const res = await fetch(`${this.apiBase}/api/orders`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (res.ok) {
        const orders = await res.json();
        const countEl = document.getElementById('cust-orders-count');
        if (countEl) countEl.textContent = orders.length;
        this.renderCustOrders(orders);
      }
    } catch (e) {
      console.error('Lỗi nạp đơn hàng:', e);
    }
  }

  async lookupOrder() {
    const input = document.getElementById('cust-lookup-input');
    const maDH = input ? input.value.trim() : '';
    if (!maDH) {
      this.showToast('Vui lòng nhập mã đơn hàng!', true);
      return;
    }
    try {
      const res = await fetch(`${this.apiBase}/api/orders/lookup?id=${encodeURIComponent(maDH)}`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      const data = await res.json();
      if (data.success && data.order) {
        this.openOrderDetailModal(data.order);
      } else {
        this.showToast(data.message || 'Không tìm thấy đơn hàng trong hệ thống!', true);
      }
    } catch (e) {
      this.showToast('Lỗi tra cứu đơn hàng.', true);
    }
  }

  renderCustOrders(orders) {
    const tbody = document.getElementById('cust-orders-tbody');
    if (!tbody) return;

    if (orders.length === 0) {
      tbody.innerHTML = `
        <tr>
          <td colspan="7" class="empty-state">
            <div class="empty-cat-img"><img src="${CAT_ICONS.envelope}" alt="Chưa có đơn" /></div>
            <h4 class="empty-title">Chưa có đơn nào, mèo đang chờ bạn đặt hàng!</h4>
            <p class="empty-sub">Hãy vào Cửa hàng để chọn sản phẩm yêu thích nhé.</p>
          </td>
        </tr>
      `;
      return;
    }

    tbody.innerHTML = orders.map(o => {
      let statusTag = '';
      let deliveryInfo = '';
      if (o.trangThaiMa === 'CHO_DUYET' || o.trangThaiMa === 'DANG_CHO_HANG') {
        statusTag = `<span class="tag-status tag-pending">${o.trangThai}</span>`;
      } else if (o.trangThaiMa === 'DA_DUYET') {
        statusTag = `<span class="tag-status tag-pending" style="background: var(--jasmine);">${o.trangThai}</span>`;
      } else if (o.trangThaiMa === 'DANG_GIAO') {
        statusTag = `<span class="tag-status tag-shipping">${o.trangThai}</span>`;
        if (o.thoiGianGiaoDuKien) {
          deliveryInfo = `<div style="font-size: 11.5px; color: var(--space-indigo-light); margin-top: 4px; line-height: 1.35;">
            🚚 Dự kiến giao: <strong>${o.thoiGianGiaoDuKien}</strong><br/>
            ⏱ Hạn phản hồi: <strong>${o.hanChotHoanThanh}</strong><br/>
            <span style="color: var(--pearl-aqua-dark); font-size: 11px;">(Tự động hoàn thành sau 3 ngày nếu không khiếu nại)</span>
          </div>`;
        }
      } else if (o.trangThaiMa === 'DA_HOAN_THANH') {
        statusTag = `<span class="tag-status tag-completed">${o.trangThai}</span>`;
      } else {
        statusTag = `<span class="tag-status tag-cancelled">${o.trangThai}</span>`;
      }

      let overdueBadge = '';
      if (o.coKhieuNai) {
        overdueBadge = `<span class="tag-status tag-cancelled">⚠️ Đang khiếu nại</span>`;
      } else if (o.quaHan3Ngay) {
        overdueBadge = `<span class="tag-status tag-overdue">⚠️ Quá hạn 3 ngày</span>`;
      }

      let actionBtns = `<button type="button" class="btn btn-outline btn-sm" onclick="App.openOrderDetailById('${o.maDonHang}')">Chi tiết</button>`;

      if (o.trangThaiMa === 'CHO_DUYET' || o.trangThaiMa === 'DANG_CHO_HANG') {
        actionBtns += ` <button type="button" class="btn btn-outline btn-sm" onclick="App.cancelCustOrder('${o.maDonHang}')" style="color: var(--princeton-orange);">Hủy đơn</button>`;
      } else if (o.trangThaiMa === 'DANG_GIAO') {
        actionBtns += `
          <button type="button" class="btn btn-primary btn-sm" onclick="App.openReviewModal('${o.maDonHang}', 'confirm')">Đã nhận hàng</button>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.openReviewModal('${o.maDonHang}', 'complain')" style="color: var(--princeton-orange);">Báo sự cố</button>
        `;
      } else if (o.trangThaiMa === 'DA_HOAN_THANH') {
        if (!o.danhGia) {
          actionBtns += ` <button type="button" class="btn btn-outline btn-sm" onclick="App.openReviewModal('${o.maDonHang}', 'review')">Đánh giá</button>`;
        }
      }

      return `
        <tr>
          <td><strong>${o.maDonHang}</strong></td>
          <td>${o.ngayDat}</td>
          <td>${o.tenKhachHang}<br/><span style="font-size: 12px; color: var(--space-indigo-light);">${o.diaChi}</span></td>
          <td><strong style="color: var(--princeton-orange);">${this.formatCurrency(o.tongTien)}</strong></td>
          <td>${statusTag}${deliveryInfo}</td>
          <td>${overdueBadge}</td>
          <td>${actionBtns}</td>
        </tr>
      `;
    }).join('');
  }

  async cancelCustOrder(maDH) {
    if (!confirm(`Bạn có chắc muốn hủy đơn hàng ${maDH}?`)) return;
    try {
      const res = await fetch(`${this.apiBase}/api/orders/cancel`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ orderId: maDH })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.loadCustOrders();
      } else {
        this.showToast(data.message || 'Không thể hủy đơn hàng.', true);
      }
    } catch (e) {
      this.showToast('Lỗi khi hủy đơn hàng.', true);
    }
  }

  // ------------------------------------------------------------------------
  // Đánh giá & Phản hồi đơn hàng
  // ------------------------------------------------------------------------
  openReviewModal(orderId, actionType) {
    document.getElementById('review-order-id').value = orderId;
    document.getElementById('review-action-type').value = actionType;
    const titleEl = document.getElementById('review-modal-title');
    const starGrp = document.getElementById('review-star-group');
    const commentLbl = document.getElementById('review-comment-label');
    const submitBtn = document.getElementById('review-submit-btn');

    if (actionType === 'complain') {
      titleEl.textContent = `Khiếu Nại Sự Cố Đơn Hàng ${orderId}`;
      starGrp.style.display = 'none';
      commentLbl.textContent = 'Mô tả chi tiết sự cố bạn gặp phải:';
      submitBtn.textContent = 'Gửi Khiếu Nại & Giữ Đơn';
    } else if (actionType === 'confirm') {
      titleEl.textContent = `Xác Nhận Đã Nhận Đơn Hàng ${orderId}`;
      starGrp.style.display = 'block';
      commentLbl.textContent = 'Ý kiến nhận xét của bạn:';
      submitBtn.textContent = 'Xác Nhận Hoàn Thành Đơn';
    } else {
      titleEl.textContent = `Đánh Giá Đơn Hàng ${orderId}`;
      starGrp.style.display = 'block';
      commentLbl.textContent = 'Ý kiến đóng góp:';
      submitBtn.textContent = 'Gửi Đánh Giá';
    }
    this.openModal('modal-review');
  }

  async submitReview(e) {
    e.preventDefault();
    const orderId = document.getElementById('review-order-id').value;
    const actionType = document.getElementById('review-action-type').value;
    const soSao = parseInt(document.getElementById('review-stars').value, 10) || 5;
    const noiDung = document.getElementById('review-content').value.trim();

    let endpoint = '/api/orders/confirm';
    if (actionType === 'complain') endpoint = '/api/orders/complain';
    else if (actionType === 'review') endpoint = '/api/orders/review';

    try {
      const res = await fetch(`${this.apiBase}${endpoint}`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ orderId, soSao, noiDung })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.closeModal('modal-review');
        this.loadCustOrders();
      } else {
        this.showToast(data.message || 'Thao tác thất bại!', true);
      }
    } catch (err) {
      this.showToast('Lỗi gửi phản hồi.', true);
    }
  }

  // ------------------------------------------------------------------------
  // Quản trị viên (Admin Views)
  // ------------------------------------------------------------------------
  showAdminTab(tab) {
    document.querySelectorAll('.admin-nav-item').forEach(b => b.classList.remove('active'));
    document.querySelectorAll('.admin-tab-pane').forEach(p => p.classList.remove('active'));

    const btn = document.getElementById(`adm-nav-${tab}`);
    const pane = document.getElementById(`adm-pane-${tab}`);
    if (btn) btn.classList.add('active');
    if (pane) pane.classList.add('active');

    if (tab === 'orders') this.loadAdminOrders();
    else if (tab === 'products') this.loadAdminProducts();
    else if (tab === 'warehouse') this.loadAdminWarehouse();
    else if (tab === 'revenues') this.loadAdminRevenues();
    else if (tab === 'users') this.loadAdminUsers();
    else if (tab === 'reviews') this.loadAdminReviews();
  }

  // Tab 1: Đơn hàng & FIFO
  async loadAdminOrders() {
    try {
      // 1. Hàng đợi FIFO
      const resQueue = await fetch(`${this.apiBase}/api/admin/orders/queue`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (resQueue.ok) {
        const queue = await resQueue.json();
        this.renderAdminQueue(queue);
      }

      // 2. Chờ nhập kho (Pending)
      const resPending = await fetch(`${this.apiBase}/api/admin/orders/pending`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (resPending.ok) {
        const pending = await resPending.json();
        this.renderAdminPending(pending);
      }

      // 3. Toàn bộ đơn hàng (qua BST)
      const resAll = await fetch(`${this.apiBase}/api/orders`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (resAll.ok) {
        this.adminOrders = await resAll.json();
        this.renderAdminAllOrders(this.adminOrders);
      }
    } catch (e) {
      console.error('Lỗi nạp đơn hàng admin:', e);
    }
  }

  renderAdminQueue(queue) {
    const tbody = document.getElementById('adm-queue-tbody');
    if (!tbody) return;
    if (queue.length === 0) {
      tbody.innerHTML = `<tr><td colspan="7" class="empty-state">Hiện không có đơn nào đang chờ duyệt.</td></tr>`;
      return;
    }
    tbody.innerHTML = queue.map((o, idx) => `
      <tr>
        <td><strong>#${idx + 1} - ${o.maDonHang}</strong></td>
        <td>${o.ngayDat}</td>
        <td>${o.tenKhachHang} (${o.sdt})</td>
        <td>${o.diaChi}</td>
        <td><strong style="color: var(--princeton-orange);">${this.formatCurrency(o.tongTien)}</strong></td>
        <td><span class="tag-status tag-pending">${o.trangThai}</span></td>
        <td>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.openOrderDetailById('${o.maDonHang}')">Chi tiết</button>
        </td>
      </tr>
    `).join('');
  }

  renderAdminPending(pending) {
    const tbody = document.getElementById('adm-pending-tbody');
    if (!tbody) return;
    if (pending.length === 0) {
      tbody.innerHTML = `<tr><td colspan="6" class="empty-state">Không có đơn nào bị treo do thiếu kho.</td></tr>`;
      return;
    }
    tbody.innerHTML = pending.map(o => `
      <tr>
        <td><strong>${o.maDonHang}</strong></td>
        <td>${o.tenKhachHang}</td>
        <td><strong style="color: var(--princeton-orange);">${this.formatCurrency(o.tongTien)}</strong></td>
        <td><span class="tag-status tag-pending">${o.trangThai}</span></td>
        <td><em>Đang chờ nhập thêm hàng vào kho</em></td>
        <td>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.openOrderDetailById('${o.maDonHang}')">Chi tiết</button>
        </td>
      </tr>
    `).join('');
  }

  renderAdminAllOrders(orders) {
    const tbody = document.getElementById('adm-all-orders-tbody');
    if (!tbody) return;
    if (orders.length === 0) {
      tbody.innerHTML = `<tr><td colspan="7" class="empty-state">Không có dữ liệu đơn hàng.</td></tr>`;
      return;
    }
    tbody.innerHTML = orders.map(o => {
      let statusTag = `<span class="tag-status tag-completed">${o.trangThai}</span>`;
      if (o.trangThaiMa === 'CHO_DUYET' || o.trangThaiMa === 'DANG_CHO_HANG') statusTag = `<span class="tag-status tag-pending">${o.trangThai}</span>`;
      else if (o.trangThaiMa === 'DA_DUYET') statusTag = `<span class="tag-status tag-pending" style="background: var(--jasmine);">${o.trangThai}</span>`;
      else if (o.trangThaiMa === 'DANG_GIAO') {
        statusTag = `<span class="tag-status tag-shipping">${o.trangThai}</span>`;
        if (o.thoiGianGiaoDuKien) {
          statusTag += `<br/><span style="font-size: 11px; color: var(--space-indigo-light);">Dự kiến: ${o.thoiGianGiaoDuKien}</span>`;
        }
      }
      else if (o.trangThaiMa === 'DA_HUY') statusTag = `<span class="tag-status tag-cancelled">${o.trangThai}</span>`;

      const overdueBadge = o.quaHan3Ngay ? `<span class="tag-status tag-overdue">⚠️ Quá hạn 3 ngày</span>` : '';
      const complainBadge = o.coKhieuNai ? `<span class="tag-status tag-cancelled">Có khiếu nại</span>` : '';

      let shipBtn = '';
      if (o.trangThaiMa === 'DA_DUYET') {
        shipBtn = `<button type="button" class="btn btn-primary btn-sm" onclick="App.shipOrder('${o.maDonHang}')">Giao hàng</button>`;
      }

      return `
        <tr>
          <td><strong>${o.maDonHang}</strong></td>
          <td>${o.ngayDat}</td>
          <td>${o.tenKhachHang}</td>
          <td><strong style="color: var(--princeton-orange);">${this.formatCurrency(o.tongTien)}</strong></td>
          <td>${statusTag}</td>
          <td>${overdueBadge} ${complainBadge}</td>
          <td>
            <button type="button" class="btn btn-outline btn-sm" onclick="App.openOrderDetailById('${o.maDonHang}')">Xem</button>
            ${shipBtn}
          </td>
        </tr>
      `;
    }).join('');
  }

  async approveNextOrder() {
    try {
      const res = await fetch(`${this.apiBase}/api/admin/orders/approve`, {
        method: 'POST',
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.loadAdminOrders();
      } else {
        this.showToast(data.message || 'Không thể duyệt đơn tiếp theo.', true);
      }
    } catch (e) {
      this.showToast('Lỗi khi duyệt đơn hàng.', true);
    }
  }

  async shipOrder(orderId) {
    try {
      const res = await fetch(`${this.apiBase}/api/admin/orders/ship`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ orderId })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.loadAdminOrders();
      } else {
        this.showToast(data.message || 'Chuyển giao hàng thất bại.', true);
      }
    } catch (e) {
      this.showToast('Lỗi khi chuyển giao hàng.', true);
    }
  }

  adminLookupOrder() {
    const q = (document.getElementById('adm-order-search').value || '').trim().toUpperCase();
    if (!q) {
      this.renderAdminAllOrders(this.adminOrders);
      return;
    }
    const filtered = this.adminOrders.filter(o => o.maDonHang.includes(q) || o.tenKhachHang.toUpperCase().includes(q));
    this.renderAdminAllOrders(filtered);
  }

  // Tab 2: Quản lý sản phẩm (CRUD)
  async loadAdminProducts() {
    try {
      const res = await fetch(`${this.apiBase}/api/products?sort=0`);
      if (res.ok) {
        const prods = await res.json();
        this.renderAdminProducts(prods);
      }
    } catch (e) {}
  }

  renderAdminProducts(prods) {
    const tbody = document.getElementById('adm-products-tbody');
    if (!tbody) return;
    tbody.innerHTML = prods.map(p => `
      <tr>
        <td><strong>${p.maSP}</strong></td>
        <td>${p.tenSP}</td>
        <td>${p.danhMuc}</td>
        <td><strong style="color: var(--princeton-orange);">${this.formatCurrency(p.donGia)}</strong></td>
        <td><span class="stock-tag" style="position: static;">${p.soLuongTonKho}</span></td>
        <td><span class="tag-status tag-pending">${p.loaiSP}</span></td>
        <td>${p.maNCC}</td>
        <td>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.openEditProductModal('${p.maSP}')">Sửa</button>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.openStockModal('${p.maSP}', '${p.tenSP}')">+ Kho</button>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.deleteProduct('${p.maSP}')" style="color: var(--princeton-orange);">Xóa</button>
        </td>
      </tr>
    `).join('');
  }

  openAddProductModal() {
    document.getElementById('product-form-title').textContent = 'Thêm Sản Phẩm Mới';
    document.getElementById('prod-is-edit').value = '0';
    document.getElementById('prod-ma').readOnly = false;
    document.getElementById('prod-ma').value = '';
    document.getElementById('prod-ten').value = '';
    document.getElementById('prod-dm').value = '';
    document.getElementById('prod-gia').value = '';
    document.getElementById('prod-ton').value = '';
    this.populateSupplierSelect('prod-ncc');
    this.handleProductTypeChange(document.getElementById('prod-loai').value);
    this.openModal('modal-product-form');
  }

  openEditProductModal(maSP) {
    const sp = this.allProducts.find(p => p.maSP === maSP);
    if (!sp) return;
    document.getElementById('product-form-title').textContent = `Sửa Sản Phẩm ${maSP}`;
    document.getElementById('prod-is-edit').value = '1';
    document.getElementById('prod-loai').value = sp.loaiSP;
    document.getElementById('prod-loai').disabled = true;
    document.getElementById('prod-ma').value = sp.maSP;
    document.getElementById('prod-ma').readOnly = true;
    document.getElementById('prod-ten').value = sp.tenSP;
    document.getElementById('prod-dm').value = sp.danhMuc;
    document.getElementById('prod-gia').value = sp.donGia;
    document.getElementById('prod-ton').value = sp.soLuongTonKho;
    this.populateSupplierSelect('prod-ncc', sp.maNCC);
    this.handleProductTypeChange(sp.loaiSP, sp);
    this.openModal('modal-product-form');
  }

  handleProductTypeChange(type, sp = {}) {
    const container = document.getElementById('prod-poly-fields');
    if (!container) return;
    if (type === 'THUONG') {
      container.innerHTML = `
        <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px;">
          <div class="form-group">
            <label class="form-label">Hạn dùng (số ngày):</label>
            <input type="number" id="prod-hsd" class="form-input" value="${sp.hanSuDung || 180}" />
          </div>
          <div class="form-group">
            <label class="form-label">Quy cách đóng gói:</label>
            <input type="text" id="prod-quycach" class="form-input" value="${sp.quyCachDongGoi || 'Gói'}" />
          </div>
        </div>
      `;
    } else if (type === 'TUOI_SONG') {
      container.innerHTML = `
        <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px;">
          <div class="form-group">
            <label class="form-label">Hạn dùng (ngày):</label>
            <input type="number" id="prod-hsd" class="form-input" value="${sp.hanSuDung || 5}" />
          </div>
          <div class="form-group">
            <label class="form-label">Nhiệt độ bảo quản (°C):</label>
            <input type="number" step="0.5" id="prod-nhietdo" class="form-input" value="${sp.nhietDoBaoQuan !== undefined ? sp.nhietDoBaoQuan : 2.0}" />
          </div>
        </div>
      `;
    } else if (type === 'CONG_NGHE') {
      container.innerHTML = `
        <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px;">
          <div class="form-group">
            <label class="form-label">Bảo hành (tháng):</label>
            <input type="number" id="prod-baohanh" class="form-input" value="${sp.thoiGianBaoHanh || 12}" />
          </div>
          <div class="form-group">
            <label class="form-label">Cần bảo quản cẩn thận:</label>
            <select id="prod-canbq" class="form-select">
              <option value="true" ${sp.canBaoQuanCanThan ? 'selected' : ''}>Có</option>
              <option value="false" ${!sp.canBaoQuanCanThan ? 'selected' : ''}>Không</option>
            </select>
          </div>
        </div>
      `;
    } else {
      container.innerHTML = `
        <div class="form-group">
          <label class="form-label">Chất liệu chế tạo:</label>
          <input type="text" id="prod-chatlieu" class="form-input" value="${sp.chatLieu || 'Inox / Hợp kim'}" />
        </div>
      `;
    }
  }

  async saveProduct(e) {
    e.preventDefault();
    const isEdit = document.getElementById('prod-is-edit').value === '1';
    const loaiSP = document.getElementById('prod-loai').value;
    const maSP = document.getElementById('prod-ma').value.trim();
    const tenSP = document.getElementById('prod-ten').value.trim();
    const danhMuc = document.getElementById('prod-dm').value.trim();
    const donGia = document.getElementById('prod-gia').value;
    const soLuongTonKho = document.getElementById('prod-ton').value;
    const maNCC = document.getElementById('prod-ncc').value;

    const payload = { loaiSP, maSP, tenSP, danhMuc, donGia, soLuongTonKho, maNCC };
    if (loaiSP === 'THUONG' || loaiSP === 'TUOI_SONG') {
      const hsd = document.getElementById('prod-hsd');
      if (hsd) payload.hanSuDung = hsd.value;
      const qc = document.getElementById('prod-quycach');
      if (qc) payload.quyCach = qc.value;
      const nd = document.getElementById('prod-nhietdo');
      if (nd) payload.nhietDo = nd.value;
    } else if (loaiSP === 'CONG_NGHE') {
      const bh = document.getElementById('prod-baohanh');
      if (bh) payload.baoHanh = bh.value;
      const cbq = document.getElementById('prod-canbq');
      if (cbq) payload.canBaoQuanCanThan = cbq.value;
    } else {
      const cl = document.getElementById('prod-chatlieu');
      if (cl) payload.chatLieu = cl.value;
    }

    const endpoint = isEdit ? '/api/admin/products/update' : '/api/admin/products/add';
    try {
      const res = await fetch(`${this.apiBase}${endpoint}`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify(payload)
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.closeModal('modal-product-form');
        this.loadAdminProducts();
        this.loadProducts();
      } else {
        this.showToast(data.message || 'Thao tác sản phẩm thất bại.', true);
      }
    } catch (err) {
      this.showToast('Lỗi gửi dữ liệu sản phẩm.', true);
    }
  }

  async deleteProduct(maSP) {
    if (!confirm(`Bạn có chắc muốn xóa sản phẩm ${maSP}?`)) return;
    try {
      const res = await fetch(`${this.apiBase}/api/admin/products/delete`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ maSP })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.loadAdminProducts();
        this.loadProducts();
      } else {
        this.showToast(data.message || 'Xóa sản phẩm thất bại.', true);
      }
    } catch (e) {
      this.showToast('Lỗi xóa sản phẩm.', true);
    }
  }

  openStockModal(maSP, tenSP) {
    document.getElementById('stock-ma-sp').value = maSP;
    document.getElementById('stock-sp-name').textContent = `${maSP} - ${tenSP}`;
    document.getElementById('stock-delta').value = '20';
    this.openModal('modal-stock-form');
  }

  async saveStock(e) {
    e.preventDefault();
    const maSP = document.getElementById('stock-ma-sp').value;
    const delta = parseInt(document.getElementById('stock-delta').value, 10) || 0;
    try {
      const res = await fetch(`${this.apiBase}/api/admin/products/stock`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ maSP, delta })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.closeModal('modal-stock-form');
        this.loadAdminProducts();
        this.loadProducts();
      } else {
        this.showToast(data.message || 'Cập nhật kho thất bại.', true);
      }
    } catch (e) {
      this.showToast('Lỗi cập nhật kho.', true);
    }
  }

  // Tab 3: Kho & Nhà cung cấp
  async loadAdminWarehouse() {
    try {
      // Cảnh báo MinHeap
      const resAlerts = await fetch(`${this.apiBase}/api/admin/warehouse/alerts`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (resAlerts.ok) {
        const alerts = await resAlerts.json();
        this.renderAdminAlerts(alerts);
      }

      // Kho hàng
      const resWh = await fetch(`${this.apiBase}/api/admin/warehouses`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (resWh.ok) {
        this.adminWarehouses = await resWh.json();
        this.renderAdminWarehouses(this.adminWarehouses);
      }

      // Nhà cung cấp
      const resSup = await fetch(`${this.apiBase}/api/admin/suppliers`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (resSup.ok) {
        this.adminSuppliers = await resSup.json();
        this.renderAdminSuppliers(this.adminSuppliers);
      }
    } catch (e) {}
  }

  renderAdminAlerts(alerts) {
    const container = document.getElementById('adm-alerts-container');
    if (!container) return;
    const expHtml = alerts.expiryAlerts.map(a => `
      <div style="padding: 8px 12px; background: var(--jasmine-light); border-radius: 8px; margin-bottom: 6px; font-size: 13px;">
        <strong>${a.maSP}</strong> - ${a.tenSP} (Còn ${a.conLaiNgay} ngày - Tồn: ${a.tonKho})
      </div>
    `).join('') || '<p class="empty-sub">Không có mặt hàng nào cận date.</p>';

    const stockHtml = alerts.stockAlerts.map(s => `
      <div style="padding: 8px 12px; background: var(--princeton-orange-light); border-radius: 8px; margin-bottom: 6px; font-size: 13px;">
        <strong>${s.maSP}</strong> - ${s.tenSP} (Tồn kho thấp: <strong>${s.tonKho}</strong>)
      </div>
    `).join('') || '<p class="empty-sub">Không có sản phẩm nào thiếu hàng.</p>';

    container.innerHTML = `
      <div>
        <h4 style="font-size: 14px; margin-bottom: 8px;">Hạn Dùng Cận Ngày (≤ 5 ngày)</h4>
        ${expHtml}
      </div>
      <div>
        <h4 style="font-size: 14px; margin-bottom: 8px;">Tồn Kho Thấp (≤ 10 chiếc)</h4>
        ${stockHtml}
      </div>
    `;
  }

  renderAdminWarehouses(whs) {
    const tbody = document.getElementById('adm-warehouses-tbody');
    if (!tbody) return;
    tbody.innerHTML = whs.map(w => `
      <tr>
        <td><strong>${w.maKho}</strong></td>
        <td>${w.viTri}</td>
        <td>${w.sucChua}</td>
        <td>${w.maSanPham} (${w.tenSanPham})</td>
        <td><strong style="color: var(--princeton-orange);">${w.tonKho}</strong></td>
        <td>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.openEditWarehouseModal('${w.maKho}')">Sửa</button>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.deleteWarehouse('${w.maKho}')" style="color: var(--princeton-orange);">Xóa</button>
        </td>
      </tr>
    `).join('');
  }

  openAddWarehouseModal() {
    document.getElementById('warehouse-form-title').textContent = 'Thêm Kho Hàng Mới';
    document.getElementById('wh-ma').value = '';
    document.getElementById('wh-ma').readOnly = false;
    document.getElementById('wh-vitri').value = '';
    document.getElementById('wh-succhua').value = '5000';
    document.getElementById('wh-masp').value = '';
    this.openModal('modal-warehouse-form');
  }

  openEditWarehouseModal(maKho) {
    const w = this.adminWarehouses.find(item => item.maKho === maKho);
    if (!w) return;
    document.getElementById('warehouse-form-title').textContent = `Sửa Kho Hàng ${maKho}`;
    document.getElementById('wh-ma').value = w.maKho;
    document.getElementById('wh-ma').readOnly = true;
    document.getElementById('wh-vitri').value = w.viTri;
    document.getElementById('wh-succhua').value = w.sucChua;
    document.getElementById('wh-masp').value = w.maSanPham;
    this.openModal('modal-warehouse-form');
  }

  async saveWarehouse(e) {
    e.preventDefault();
    const payload = {
      maKho: document.getElementById('wh-ma').value.trim(),
      viTri: document.getElementById('wh-vitri').value.trim(),
      sucChua: document.getElementById('wh-succhua').value,
      maSanPham: document.getElementById('wh-masp').value.trim()
    };
    try {
      const res = await fetch(`${this.apiBase}/api/admin/warehouses/save`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify(payload)
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.closeModal('modal-warehouse-form');
        this.loadAdminWarehouse();
      } else {
        this.showToast(data.message || 'Lưu kho thất bại.', true);
      }
    } catch (err) {
      this.showToast('Lỗi gửi dữ liệu kho.', true);
    }
  }

  async deleteWarehouse(maKho) {
    if (!confirm(`Bạn có chắc muốn xóa kho ${maKho}?`)) return;
    try {
      const res = await fetch(`${this.apiBase}/api/admin/warehouses/delete`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ maKho })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.loadAdminWarehouse();
      } else {
        this.showToast(data.message || 'Xóa kho thất bại.', true);
      }
    } catch (e) {
      this.showToast('Lỗi xóa kho.', true);
    }
  }

  renderAdminSuppliers(sups) {
    const tbody = document.getElementById('adm-suppliers-tbody');
    if (!tbody) return;
    tbody.innerHTML = sups.map(s => `
      <tr>
        <td><strong>${s.maNCC}</strong></td>
        <td>${s.tenCongTy}</td>
        <td>${s.diaChi}</td>
        <td>${s.sdt}</td>
        <td>${s.danhMuc}</td>
        <td>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.openEditSupplierModal('${s.maNCC}')">Sửa</button>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.deleteSupplier('${s.maNCC}')" style="color: var(--princeton-orange);">Xóa</button>
        </td>
      </tr>
    `).join('');
  }

  openAddSupplierModal() {
    document.getElementById('supplier-form-title').textContent = 'Thêm Nhà Cung Cấp Mới';
    document.getElementById('ncc-ma').value = '';
    document.getElementById('ncc-ma').readOnly = false;
    document.getElementById('ncc-ten').value = '';
    document.getElementById('ncc-diachi').value = '';
    document.getElementById('ncc-sdt').value = '';
    document.getElementById('ncc-danhmuc').value = '';
    this.openModal('modal-supplier-form');
  }

  openEditSupplierModal(maNCC) {
    const s = this.adminSuppliers.find(item => item.maNCC === maNCC);
    if (!s) return;
    document.getElementById('supplier-form-title').textContent = `Sửa Nhà Cung Cấp ${maNCC}`;
    document.getElementById('ncc-ma').value = s.maNCC;
    document.getElementById('ncc-ma').readOnly = true;
    document.getElementById('ncc-ten').value = s.tenCongTy;
    document.getElementById('ncc-diachi').value = s.diaChi;
    document.getElementById('ncc-sdt').value = s.sdt;
    document.getElementById('ncc-danhmuc').value = s.danhMuc;
    this.openModal('modal-supplier-form');
  }

  async saveSupplier(e) {
    e.preventDefault();
    const payload = {
      maNCC: document.getElementById('ncc-ma').value.trim(),
      tenCongTy: document.getElementById('ncc-ten').value.trim(),
      diaChi: document.getElementById('ncc-diachi').value.trim(),
      sdt: document.getElementById('ncc-sdt').value.trim(),
      danhMuc: document.getElementById('ncc-danhmuc').value.trim()
    };
    try {
      const res = await fetch(`${this.apiBase}/api/admin/suppliers/save`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify(payload)
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.closeModal('modal-supplier-form');
        this.loadAdminWarehouse();
      } else {
        this.showToast(data.message || 'Lưu nhà cung cấp thất bại.', true);
      }
    } catch (err) {
      this.showToast('Lỗi gửi dữ liệu nhà cung cấp.', true);
    }
  }

  async deleteSupplier(maNCC) {
    if (!confirm(`Bạn có chắc muốn xóa nhà cung cấp ${maNCC}?`)) return;
    try {
      const res = await fetch(`${this.apiBase}/api/admin/suppliers/delete`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ maNCC })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.loadAdminWarehouse();
      } else {
        this.showToast(data.message || 'Xóa nhà cung cấp thất bại.', true);
      }
    } catch (e) {
      this.showToast('Lỗi xóa nhà cung cấp.', true);
    }
  }

  populateSupplierSelect(selectId, selectedVal = '') {
    const sel = document.getElementById(selectId);
    if (!sel) return;
    const sups = this.adminSuppliers.length > 0 ? this.adminSuppliers : [
      { maNCC: 'NCC01', tenCongTy: 'Gạo & Nông sản Việt' },
      { maNCC: 'NCC02', tenCongTy: 'Acecook Việt Nam' },
      { maNCC: 'NCC03', tenCongTy: 'Masan Consumer' },
      { maNCC: 'NCC04', tenCongTy: 'Vinamilk' },
      { maNCC: 'NCC06', tenCongTy: 'Biển Đông Hải Sản' },
      { maNCC: 'NCC09', tenCongTy: 'FPT Digital Retail' },
      { maNCC: 'NCC10', tenCongTy: 'Sunhouse Việt Nam' }
    ];
    sel.innerHTML = sups.map(s => `
      <option value="${s.maNCC}" ${s.maNCC === selectedVal ? 'selected' : ''}>${s.maNCC} - ${s.tenCongTy}</option>
    `).join('');
  }

  // Tab 4: Doanh thu & Báo cáo & Biểu đồ cột SVG
  async loadAdminRevenues(startDate = '', endDate = '') {
    try {
      let url = `${this.apiBase}/api/admin/revenues`;
      if (startDate || endDate) {
        url += `?startDate=${encodeURIComponent(startDate)}&endDate=${encodeURIComponent(endDate)}`;
      }
      const res = await fetch(url, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (res.ok) {
        const data = await res.json();
        this.renderAdminRevenues(data);
      }
    } catch (e) {
      console.error('Lỗi nạp doanh thu:', e);
    }
  }

  filterRevenues() {
    const start = document.getElementById('rev-start-date').value;
    const end = document.getElementById('rev-end-date').value;
    this.loadAdminRevenues(start, end);
  }

  resetRevenueFilter() {
    document.getElementById('rev-start-date').value = '';
    document.getElementById('rev-end-date').value = '';
    this.loadAdminRevenues();
  }

  renderAdminRevenues(data) {
    document.getElementById('kpi-total-revenue').textContent = this.formatCurrency(data.tongDoanhThu);
    document.getElementById('kpi-total-orders').textContent = data.tongSoDon;
    document.getElementById('kpi-total-days').textContent = data.soNgay;

    // Lam tron so nguyen chuan tien te VND tu thuat toan backend
    const avg = data.trungBinh !== undefined ? Math.round(data.trungBinh) : (data.danhSach && data.danhSach.length > 0 ? Math.round(data.tongDoanhThu / data.danhSach.length) : 0);
    document.getElementById('kpi-avg-revenue').textContent = this.formatCurrency(avg);

    // Ve bieu do cot ket hop duong xu huong (Linear Regression Trendline)
    this.renderRevenueChart(data);

    // Bang doanh thu
    const tbody = document.getElementById('adm-revenues-tbody');
    if (!tbody) return;
    if (!data.danhSach || data.danhSach.length === 0) {
      tbody.innerHTML = `<tr><td colspan="3" class="empty-state">Không có dữ liệu doanh thu trong khoảng thời gian này.</td></tr>`;
      return;
    }
    tbody.innerHTML = data.danhSach.map(d => `
      <tr>
        <td><strong>${d.ngay}</strong></td>
        <td><strong style="color: var(--princeton-orange);">${this.formatCurrency(d.doanhThu)}</strong></td>
        <td>${d.soDon} đơn hàng</td>
      </tr>
    `).join('');
  }

  renderRevenueChart(data) {
    const container = document.getElementById('revenue-chart-svg');
    if (!container) return;
    const days = data.danhSach || [];
    if (!days || days.length === 0) {
      container.innerHTML = '<p class="empty-sub">Chưa có dữ liệu vẽ biểu đồ.</p>';
      return;
    }

    const maxRev = Math.max(...days.map(d => Math.max(d.doanhThu, d.trendValue || 0)), 100000);
    const chartHeight = 160;
    const chartWidth = 620;
    const barWidth = Math.min(38, Math.max(16, (chartWidth / days.length) - 14));

    const trendPoints = [];
    const bars = days.map((d, idx) => {
      const h = (d.doanhThu / maxRev) * chartHeight;
      const x = 35 + idx * (chartWidth / days.length);
      const y = 180 - h;

      // Diem tren duong xu huong hoi quy tuyen tinh
      const tVal = (d.trendValue !== undefined) ? d.trendValue : d.doanhThu;
      const tH = (tVal / maxRev) * chartHeight;
      const tX = x + barWidth / 2;
      const tY = Math.max(20, Math.min(180, 180 - tH));
      trendPoints.push(`${tX},${tY}`);

      return `
        <rect x="${x}" y="${y}" width="${barWidth}" height="${h}" rx="6" fill="var(--princeton-orange)">
          <title>${d.ngay}: ${this.formatCurrency(d.doanhThu)} (${d.soDon} đơn)</title>
        </rect>
        <text x="${x + barWidth / 2}" y="198" font-size="10.5" font-weight="700" fill="var(--space-indigo)" text-anchor="middle">
          ${d.ngay.slice(5)}
        </text>
      `;
    }).join('');

    const trendLineSvg = (trendPoints.length > 1) ? `
      <polyline points="${trendPoints.join(' ')}" fill="none" stroke="var(--space-indigo)" stroke-width="2.5" stroke-dasharray="5,4" />
      ${trendPoints.map(p => {
        const [cx, cy] = p.split(',');
        return `<circle cx="${cx}" cy="${cy}" r="3.5" fill="var(--pearl-aqua)" stroke="var(--space-indigo)" stroke-width="2" />`;
      }).join('')}
    ` : '';

    const badgeInfo = data.nhanXuHuong ? `
      <div style="display:flex; flex-wrap:wrap; align-items:center; justify-content:space-between; gap:12px; margin-bottom:10px; font-size:12.5px; color:var(--space-indigo); padding:6px 12px; background:var(--jasmine-light); border-radius:var(--radius-sm);">
        <div>📈 <strong>Xu hướng:</strong> <span style="color:var(--princeton-orange); font-weight:700;">${data.nhanXuHuong}</span> (${data.heSoGoc > 0 ? '+' : ''}${Math.round(data.heSoGoc || 0).toLocaleString('vi-VN')} đ/ngày)</div>
        <div>🎯 <strong>Dự báo ngày kế:</strong> <strong>${this.formatCurrency(data.duBaoNgayKe || 0)}</strong></div>
        <div style="font-size:11.5px; color:var(--space-indigo-light);">📐 Thuật toán: Hồi quy tuyến tính (OLS) & QuickSort</div>
      </div>
    ` : '';

    container.innerHTML = `
      ${badgeInfo}
      <svg viewBox="0 0 ${chartWidth + 50} 215" class="chart-svg" style="width:100%; height:auto;">
        <line x1="20" y1="180" x2="${chartWidth + 40}" y2="180" stroke="var(--jasmine)" stroke-width="2" />
        ${bars}
        ${trendLineSvg}
      </svg>
    `;
  }

  // Tab 5: Quản lý tài khoản (CRUD)
  async loadAdminUsers() {
    try {
      const res = await fetch(`${this.apiBase}/api/admin/users`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (res.ok) {
        this.adminUsers = await res.json();
        this.renderAdminUsers(this.adminUsers);
      }
    } catch (e) {}
  }

  renderAdminUsers(users) {
    const tbody = document.getElementById('adm-users-tbody');
    if (!tbody) return;
    const currentId = this.currentUser ? this.currentUser.id : '';
    const currentName = this.currentUser ? this.currentUser.username : '';
    tbody.innerHTML = users.map(u => {
      const isSelf = (u.id === currentId || u.username === currentName);
      const deleteAction = isSelf
        ? `<span style="font-size: 11.5px; color: var(--space-indigo-light); font-weight: 500; padding: 4px 6px;" title="Tài khoản hiện tại của bạn không thể tự xóa">(Đang dùng)</span>`
        : `<button type="button" class="btn btn-outline btn-sm" onclick="App.deleteUser('${u.id}')" style="color: var(--princeton-orange);">Xóa</button>`;
      return `
      <tr>
        <td><strong>${u.id}</strong></td>
        <td>${u.username}</td>
        <td><span class="tag-status ${u.role === 'ADMIN' ? 'tag-shipping' : 'tag-pending'}">${u.role}</span></td>
        <td>${u.hoTen}</td>
        <td>${u.soDienThoai}</td>
        <td>${u.diaChi}</td>
        <td>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.openEditUserModal('${u.id}')">Sửa</button>
          <button type="button" class="btn btn-outline btn-sm" onclick="App.openResetPasswordModal('${u.id}', '${u.username}')">Đổi MK</button>
          ${deleteAction}
        </td>
      </tr>
    `}).join('');
  }

  openAddUserModal() {
    this.isRegisterMode = false;
    this.usernameExists = false;
    document.getElementById('user-form-title').textContent = 'Thêm Tài Khoản Mới';
    document.getElementById('usr-id').value = '';
    document.getElementById('usr-role').disabled = false;
    document.getElementById('usr-role').value = 'CUSTOMER';
    const usrName = document.getElementById('usr-username');
    if (usrName) {
      usrName.value = '';
      usrName.readOnly = false;
      usrName.style.borderColor = '';
    }
    const feedback = document.getElementById('usr-username-feedback');
    if (feedback) { feedback.style.display = 'none'; feedback.textContent = ''; }
    document.getElementById('usr-password-group').style.display = 'block';
    document.getElementById('usr-password').value = '';
    document.getElementById('usr-password').required = true;
    document.getElementById('usr-hoten').value = '';
    document.getElementById('usr-sdt').value = '';
    document.getElementById('usr-diachi').value = '';
    const submitBtn = document.querySelector('#form-user button[type="submit"]');
    if (submitBtn) submitBtn.textContent = 'Lưu Tài Khoản';
    this.openModal('modal-user-form');
  }

  openEditUserModal(userId) {
    this.isRegisterMode = false;
    this.usernameExists = false;
    const u = this.adminUsers.find(item => item.id === userId);
    if (!u) return;
    document.getElementById('user-form-title').textContent = `Sửa Tài Khoản ${u.username}`;
    document.getElementById('usr-id').value = u.id;
    document.getElementById('usr-role').value = u.role;
    document.getElementById('usr-role').disabled = true;
    const usrName = document.getElementById('usr-username');
    if (usrName) {
      usrName.value = u.username;
      usrName.readOnly = true;
      usrName.style.borderColor = '';
    }
    const feedback = document.getElementById('usr-username-feedback');
    if (feedback) { feedback.style.display = 'none'; feedback.textContent = ''; }
    document.getElementById('usr-password-group').style.display = 'none';
    document.getElementById('usr-password').required = false;
    document.getElementById('usr-hoten').value = u.hoTen;
    document.getElementById('usr-sdt').value = u.soDienThoai;
    document.getElementById('usr-diachi').value = u.diaChi;
    const submitBtn = document.querySelector('#form-user button[type="submit"]');
    if (submitBtn) submitBtn.textContent = 'Cập Nhật Tài Khoản';
    this.openModal('modal-user-form');
  }

  async checkUsernameRealtime(val) {
    const feedback = document.getElementById('usr-username-feedback');
    const input = document.getElementById('usr-username');
    if (!feedback || !input) return;

    if (input.readOnly) {
      feedback.style.display = 'none';
      return;
    }

    if (!val) {
      feedback.style.display = 'none';
      input.style.borderColor = '';
      this.usernameExists = false;
      return;
    }

    if (val.length < 3) {
      feedback.style.display = 'block';
      feedback.style.color = '#64748b';
      feedback.textContent = 'ℹ️ Tên đăng nhập phải từ 3 đến 30 ký tự.';
      input.style.borderColor = '';
      this.usernameExists = false;
      return;
    }

    const regex = /^[a-zA-Z0-9_]+$/;
    if (!regex.test(val)) {
      feedback.style.display = 'block';
      feedback.style.color = '#ef4444';
      feedback.textContent = '⚠️ Chỉ gồm chữ cái không dấu, chữ số và dấu gạch dưới.';
      input.style.borderColor = '#ef4444';
      this.usernameExists = true;
      return;
    }

    try {
      const res = await fetch(`${this.apiBase}/api/check-username?username=${encodeURIComponent(val)}`);
      if (res.ok) {
        const data = await res.json();
        feedback.style.display = 'block';
        if (data.exists) {
          feedback.style.color = '#ef4444';
          feedback.textContent = '❌ Tên đăng nhập đã tồn tại (phát hiện bởi cây Trie)! Vui lòng chọn tên khác.';
          input.style.borderColor = '#ef4444';
          this.usernameExists = true;
        } else {
          feedback.style.color = '#10b981';
          feedback.textContent = '✅ Tên đăng nhập hợp lệ và có thể sử dụng (kiểm tra qua cây Trie).';
          input.style.borderColor = '#10b981';
          this.usernameExists = false;
        }
      }
    } catch (e) {
      console.warn('Lỗi kiểm tra tên đăng nhập qua Trie:', e);
    }
  }

  async saveUser(e) {
    e.preventDefault();
    const id = document.getElementById('usr-id').value;
    const role = document.getElementById('usr-role').value;
    const username = document.getElementById('usr-username').value.trim();
    const password = document.getElementById('usr-password').value;
    const hoTen = document.getElementById('usr-hoten').value.trim();
    const soDienThoai = document.getElementById('usr-sdt').value.trim();
    const diaChi = document.getElementById('usr-diachi').value.trim();

    if ((this.isRegisterMode || !id) && this.usernameExists) {
      this.showToast('Tên đăng nhập đã tồn tại! Vui lòng chọn tên khác.', true);
      return;
    }

    if (this.isRegisterMode || !this.token) {
      // Đăng ký khách hàng (Public API)
      try {
        const res = await fetch(`${this.apiBase}/api/register`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ username, password, hoTen, soDienThoai, diaChi })
        });
        const data = await res.json();
        if (data.success) {
          this.showToast(data.message);
          this.closeModal('modal-user-form');
          // Điền tên vào ô đăng nhập để tiện đăng nhập ngay
          const loginUser = document.getElementById('login-username');
          if (loginUser) loginUser.value = username;
          const loginPass = document.getElementById('login-password');
          if (loginPass) { loginPass.value = ''; loginPass.focus(); }
          this.selectRole('customer');
        } else {
          this.showToast(data.message || 'Đăng ký tài khoản thất bại.', true);
        }
      } catch (err) {
        this.showToast('Lỗi gửi dữ liệu đăng ký.', true);
      }
      return;
    }

    try {
      const res = await fetch(`${this.apiBase}/api/admin/users/save`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ id, role, username, password, hoTen, soDienThoai, diaChi })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.closeModal('modal-user-form');
        this.loadAdminUsers();
      } else {
        this.showToast(data.message || 'Lưu tài khoản thất bại.', true);
      }
    } catch (err) {
      this.showToast('Lỗi gửi dữ liệu tài khoản.', true);
    }
  }

  async deleteUser(userId) {
    if (!confirm(`Bạn có chắc muốn xóa tài khoản ID ${userId}?`)) return;
    try {
      const res = await fetch(`${this.apiBase}/api/admin/users/delete`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ id: userId })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.loadAdminUsers();
      } else {
        this.showToast(data.message || 'Xóa tài khoản thất bại.', true);
      }
    } catch (e) {
      this.showToast('Lỗi xóa tài khoản.', true);
    }
  }

  openResetPasswordModal(userId, username) {
    document.getElementById('reset-user-id').value = userId;
    document.getElementById('reset-user-info').textContent = `Tài khoản: ${username} (ID: ${userId})`;
    document.getElementById('reset-new-password').value = '';
    this.openModal('modal-password-form');
  }

  async saveResetPassword(e) {
    e.preventDefault();
    const userId = document.getElementById('reset-user-id').value;
    const newPassword = document.getElementById('reset-new-password').value;
    try {
      const res = await fetch(`${this.apiBase}/api/admin/users/password`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${this.token}`
        },
        body: JSON.stringify({ userId, newPassword })
      });
      const data = await res.json();
      if (data.success) {
        this.showToast(data.message);
        this.closeModal('modal-password-form');
      } else {
        this.showToast(data.message || 'Đổi mật khẩu thất bại.', true);
      }
    } catch (e) {
      this.showToast('Lỗi đổi mật khẩu.', true);
    }
  }

  // Tab 6: Đánh giá & Khiếu nại
  async loadAdminReviews() {
    try {
      const res = await fetch(`${this.apiBase}/api/admin/reviews`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      if (res.ok) {
        const reviews = await res.json();
        this.renderAdminReviews(reviews);
      }
    } catch (e) {}
  }

  renderAdminReviews(reviews) {
    const tbody = document.getElementById('adm-reviews-tbody');
    if (!tbody) return;
    if (reviews.length === 0) {
      tbody.innerHTML = `<tr><td colspan="7" class="empty-state">Chưa có đánh giá hoặc khiếu nại nào từ khách hàng.</td></tr>`;
      return;
    }
    tbody.innerHTML = reviews.map(r => `
      <tr>
        <td><strong>${r.maDanhGia}</strong></td>
        <td><a href="javascript:void(0)" onclick="App.openOrderDetailById('${r.maDonHang}')">${r.maDonHang}</a></td>
        <td>${r.tenKhachHang}</td>
        <td><strong style="color: var(--princeton-orange);">${'⭐'.repeat(r.soSao)}</strong> (${r.soSao}/5)</td>
        <td>${r.noiDung || '<em>Không có nhận xét</em>'}</td>
        <td>
          <span class="tag-status ${r.laKhieuNai ? 'tag-cancelled' : 'tag-completed'}">
            ${r.laKhieuNai ? 'Khiếu nại sự cố' : 'Đánh giá hài lòng'}
          </span>
        </td>
        <td>${r.thoiGian}</td>
      </tr>
    `).join('');
  }

  // ------------------------------------------------------------------------
  // Modals Chi tiết sản phẩm & Đơn hàng
  // ------------------------------------------------------------------------
  async openProductDetail(productId) {
    try {
      const res = await fetch(`${this.apiBase}/api/products/detail?id=${encodeURIComponent(productId)}`);
      if (res.ok) {
        const sp = await res.json();
        document.getElementById('detail-product-title').textContent = `${sp.maSP} - ${sp.tenSP}`;
        let polyHtml = '';
        if (sp.loaiSP === 'THUONG') {
          polyHtml = `
            <p><strong>Loại:</strong> Thực phẩm thường</p>
            <p><strong>Hạn dùng:</strong> ${sp.hanSuDung} ngày (còn lại ~${sp.conLaiNgay} ngày)</p>
            <p><strong>Quy cách đóng gói:</strong> ${sp.quyCachDongGoi}</p>
          `;
        } else if (sp.loaiSP === 'TUOI_SONG') {
          polyHtml = `
            <p><strong>Loại:</strong> Thực phẩm tươi sống</p>
            <p><strong>Hạn dùng:</strong> ${sp.hanSuDung} ngày (còn lại ~${sp.conLaiNgay} ngày)</p>
            <p><strong>Nhiệt độ bảo quản:</strong> ${sp.nhietDoBaoQuan} °C</p>
          `;
        } else if (sp.loaiSP === 'CONG_NGHE') {
          polyHtml = `
            <p><strong>Loại:</strong> Thiết bị công nghệ & điện tử</p>
            <p><strong>Bảo hành:</strong> ${sp.thoiGianBaoHanh} tháng</p>
            <p><strong>Cần bảo quản cẩn thận:</strong> ${sp.canBaoQuanCanThan ? 'Có (chống va đập)' : 'Không'}</p>
          `;
        } else {
          polyHtml = `
            <p><strong>Loại:</strong> Đồ gia dụng</p>
            <p><strong>Chất liệu:</strong> ${sp.chatLieu}</p>
          `;
        }

        document.getElementById('detail-product-body').innerHTML = `
          <div style="font-size: 14px; line-height: 1.8;">
            <p><strong>Mã sản phẩm:</strong> ${sp.maSP}</p>
            <p><strong>Tên sản phẩm:</strong> ${sp.tenSP}</p>
            <p><strong>Danh mục:</strong> ${sp.danhMuc}</p>
            <p><strong>Đơn giá:</strong> <span style="font-weight: 800; color: var(--princeton-orange);">${this.formatCurrency(sp.donGia)}</span></p>
            <p><strong>Số lượng tồn kho:</strong> ${sp.soLuongTonKho} chiếc</p>
            <p><strong>Mã nhà cung cấp:</strong> ${sp.maNCC}</p>
            <hr style="margin: 12px 0; border: none; border-top: 1px solid var(--jasmine-light);" />
            ${polyHtml}
          </div>
          <div style="margin-top: 20px;">
            <button type="button" class="btn btn-primary btn-block" onclick="App.addToCart('${sp.maSP}'); App.closeModal('modal-product-detail');">
              + Thêm Vào Giỏ Hàng
            </button>
          </div>
        `;
        this.openModal('modal-product-detail');
      }
    } catch (e) {
      this.showToast('Không thể lấy chi tiết sản phẩm.', true);
    }
  }

  async openOrderDetailById(orderId) {
    try {
      const res = await fetch(`${this.apiBase}/api/orders/lookup?id=${encodeURIComponent(orderId)}`, {
        headers: { 'Authorization': `Bearer ${this.token}` }
      });
      const data = await res.json();
      if (data.success && data.order) {
        this.openOrderDetailModal(data.order);
      } else {
        this.showToast(data.message || 'Không tìm thấy chi tiết đơn hàng.', true);
      }
    } catch (e) {
      this.showToast('Lỗi xem chi tiết đơn hàng.', true);
    }
  }

  openOrderDetailModal(order) {
    document.getElementById('detail-order-title').textContent = `Chi Tiết Đơn Hàng ${order.maDonHang}`;
    const itemsHtml = order.chiTiet.map(ct => `
      <tr>
        <td><strong>${ct.maSP}</strong> - ${ct.tenSP}</td>
        <td>${ct.soLuong}</td>
        <td>${this.formatCurrency(ct.donGia)}</td>
        <td><strong style="color: var(--princeton-orange);">${this.formatCurrency(ct.thanhTien)}</strong></td>
      </tr>
    `).join('');

    let reviewHtml = '';
    if (order.danhGia) {
      reviewHtml = `
        <div style="margin-top: 16px; padding: 12px; background: var(--jasmine-light); border-radius: 8px;">
          <strong>Phản hồi của khách hàng:</strong>
          <div>Số sao: <strong>${'⭐'.repeat(order.danhGia.soSao)}</strong> (${order.danhGia.soSao}/5)</div>
          <div>Ý kiến: ${order.danhGia.noiDung || '<em>Không có</em>'}</div>
          <div>Loại: <strong>${order.danhGia.khieuNai ? 'Khiếu nại sự cố' : 'Đánh giá bình thường'}</strong></div>
        </div>
      `;
    }

    document.getElementById('detail-order-body').innerHTML = `
      <div style="font-size: 13.5px; line-height: 1.8; margin-bottom: 16px;">
        <p><strong>Mã đơn hàng:</strong> ${order.maDonHang}</p>
        <p><strong>Ngày đặt:</strong> ${order.ngayDat}</p>
        <p><strong>Người nhận:</strong> ${order.tenKhachHang} (${order.sdt})</p>
        <p><strong>Địa chỉ:</strong> ${order.diaChi}</p>
        <p><strong>Trạng thái:</strong> <span class="tag-status tag-pending">${order.trangThai}</span></p>
      </div>
      <h4 style="font-size: 14px; margin-bottom: 8px;">Danh Sách Mặt Hàng (ChiTietDonHang):</h4>
      <div class="table-wrap">
        <table class="data-table">
          <thead>
            <tr>
              <th>Sản Phẩm</th>
              <th>Số Lượng</th>
              <th>Đơn Giá</th>
              <th>Thành Tiền</th>
            </tr>
          </thead>
          <tbody>
            ${itemsHtml}
          </tbody>
        </table>
      </div>
      <div style="display: flex; justify-content: space-between; align-items: center; margin-top: 16px; font-size: 16px; font-weight: 800;">
        <span>Tổng giá trị đơn:</span>
        <span style="color: var(--princeton-orange);">${this.formatCurrency(order.tongTien)}</span>
      </div>
      ${reviewHtml}
    `;
    this.openModal('modal-order-detail');
  }

  // Tiện ích đóng mở Modal
  openModal(modalId) {
    const m = document.getElementById(modalId);
    if (m) m.classList.add('active');
  }

  closeModal(modalId) {
    const m = document.getElementById(modalId);
    if (m) m.classList.remove('active');
  }
}

// Khởi chạy App toàn cục
window.App = new MeoLogiApp();
window.addEventListener('DOMContentLoaded', () => {
  window.App.init();
});
